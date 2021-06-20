#include <iostream>
#include <iterator>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <cpr/cpr.h>
#include <tidybuffio.h>
#include <pugixml.hpp>


class threadSafe_uMap {
    mutable std::unordered_map<std::string, std::string> m_sMap;
    mutable std::mutex m_mutex;

public:
    threadSafe_uMap() = default;
    std::string operator [](std::string s) const {
        std::string retVal;
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        retVal = m_sMap[s];
        return retVal;
    };

    std::string & operator [](std::string s) {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        return m_sMap[s];
    };

    void set(std::string k, std::string v){
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        m_sMap[k] = v;
        return;
    }

    std::string get(std::string k){
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        return m_sMap[k];
    }

};

std::string tokenize(std::string const &str, const char startDelim = '<', const char endDelim = '>')
{
    size_t start;
    size_t end = 0;
    std::string retVal;

    while ((start = str.find_first_of(startDelim, end)) != std::string::npos)
    {
        end = str.find(endDelim, start);
        retVal.append(str.substr(start, end + 1 - start));
    }
    return retVal;
}

void tidyFindChildren(const TidyNode &node, int &nodeNum, int &leafNode, int &divNode){
    
    nodeNum++;
    if(tidyNodeGetName(node) && "div" == std::string(tidyNodeGetName(node))){
        ++divNode;
    }
    auto child = tidyGetChild(node);
    if(!child){
        leafNode++;
    }
    while(child){
        tidyFindChildren(child, nodeNum, leafNode, divNode);

        child = tidyGetNext(child);
    }
    return;
}

void findChildren(const pugi::xml_node &node, int &nodeNum, int &leafNode){
    ++nodeNum;
    auto children = node.children();
    if(children.begin() == children.end()){
        leafNode++;
    }else{
        for(const auto & child: children){
            findChildren(child, nodeNum, leafNode);
        }
    }
    return;
}
void foo(const std::string & URL, threadSafe_uMap &smap){
    cpr::Response r = cpr::Get(cpr::Url{URL});
    int retryCount(0);
    while((r.status_code < 200 || r.status_code > 299) && retryCount < 3){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        r = cpr::Get(cpr::Url{URL});
        ++retryCount;
    };    
    TidyDoc tidyDoc = tidyCreate();
    tidyOptSetBool(tidyDoc, TidyXhtmlOut, yes)
      && tidyOptSetBool(tidyDoc, TidyQuiet, yes)
      && tidyOptSetBool(tidyDoc, TidyNumEntities, yes)
      && tidyOptSetBool(tidyDoc, TidyShowWarnings, no)
      && tidyOptSetBool(tidyDoc, TidyDropEmptyElems, no)
      && tidyOptSetInt(tidyDoc, TidyWrapLen, 0)
      && tidyOptSetInt(tidyDoc, TidyUseCustomTags, 4);
    tidyParseString(tidyDoc, r.text.c_str());
    auto html = tidyGetHtml(tidyDoc);

    int nodeNum(0);
    int leafNum(0);
    int divNum(0);
    tidyFindChildren(html, nodeNum, leafNum, divNum);
    TidyBuffer tidyOutputBuffer = {0};
    pugi::xml_document xmlDoc;
    tidySaveBuffer(tidyDoc, &tidyOutputBuffer);
    if(tidyOutputBuffer.bp) {
      std::string tidyResult;
      tidyResult = (char*)tidyOutputBuffer.bp;
      tidyBufFree(&tidyOutputBuffer);
      pugi::xml_parse_result result = xmlDoc.load_buffer(tidyResult.c_str(), tidyResult.size());
    }
    tidyRelease(tidyDoc);

    std::stringstream msg;
    msg << URL << "\t" << nodeNum << "\t" << leafNum<< "\t" << divNum <<"\n";
    smap[URL] = msg.str();
}

void fooRunner(threadSafe_uMap &smap, std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end){
    for(auto it = start; it != end; std::advance(it, 1)){
        foo(*it, smap);
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> urls;
    size_t numThreads;
    if(argc < 2){
        urls = {"https://example.com/"};
        numThreads = 1;
    }
    else{
        std::ifstream input(argv[1]);
        std::string line;
        while(std::getline(input, line)){
            if(line.size()>0){
                urls.push_back(line);
            }
        }
        if(argc < 3){
            numThreads = 1;
        }else{
            numThreads = std::stoi(argv[2]);
        }

    }
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    threadSafe_uMap smap;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);    
    size_t length = urls.size() / numThreads;
    size_t remain = urls.size() % numThreads;
    size_t start(0);
    size_t end(0);
    for(size_t i = 0; i < std::min<size_t>(numThreads, urls.size()); ++i){
        end += (remain > 0) ? (length + !!(remain--)) : length;
        threads.emplace_back(std::thread(fooRunner, std::ref(smap), std::next(urls.begin(), start), std::next(urls.begin(), end) ));
        start = end;
    }
    for (auto & th: threads){
        th.join();
    }
    for(const auto & url: urls){
        std::cout << smap[url];
    }

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_double.count() << "ms\n";
    return 0;
}