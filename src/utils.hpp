#include "../include/httpreq/httplib.h"
#include "../include/logger/loggand.hpp"
#include "../config.hpp"


int fetchNews(std::string api_key){
    std::string API_URL = "/v1/news?access_key=" + api_key;
    Logger log = Logger("nil",LOG_LEVEL::DEBUG);
    try{
        httplib::Client client(MEDIASTACK_API_URL);
        if(auto res = client.Get(API_URL)){
            res->status;
            res->body;
            log.message = "API call successful\n";
            log.status = LOG_LEVEL::SUCCESS;
            log.log();
            return 1;
        }else {
            throw;
        }
    }
    catch(const std::exception &err){
        log.message = err.what();
        log.status = LOG_LEVEL::ERROR;
        log.log();
        return -1;
    }
}