#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma once

#include <string>
#include <stdexcept>
#include "../include/httpreq/httplib.h"
#include "../include/logger/loggand.hpp"
#include "../include/json/json.hpp"
#include "../config.hpp"
#include "./newsModel.hpp"

json::Prettifier prettifier;

int storeToDB(std::string api_key, const std::string jsonBody) {
    std::string API_URL = "/rest/v1/news";
    Logger log = Logger("nil", LOG_LEVEL::DEBUG);
    try {
        httplib::Client client(SUPABASE_API_URL);
        httplib::Headers headers = {
            {"apikey", api_key},
            {"Authorization", "Bearer " + api_key},
            {"Content-Type", "application/json"}
        };
        
        // Extract the first news title by manually parsing the JSON string
        std::string firstTitle;
        
        // Look for "data" array in the JSON
        size_t dataPos = jsonBody.find("\"data\"");
        if (dataPos != std::string::npos) {
            // Find the first object in the data array
            size_t arrayStart = jsonBody.find("[", dataPos);
            if (arrayStart != std::string::npos) {
                // Find the first opening brace of the first object
                size_t firstObjectStart = jsonBody.find("{", arrayStart);
                if (firstObjectStart != std::string::npos) {
                    // Find "title" field within the first object
                    size_t titlePos = jsonBody.find("\"title\"", firstObjectStart);
                    if (titlePos != std::string::npos && titlePos < jsonBody.find("}", firstObjectStart)) {
                        // Find the colon after "title"
                        size_t colonPos = jsonBody.find(":", titlePos);
                        if (colonPos != std::string::npos) {
                            // Find the opening quote of the title value
                            size_t quoteStart = jsonBody.find("\"", colonPos);
                            if (quoteStart != std::string::npos) {
                                // Find the closing quote
                                size_t quoteEnd = jsonBody.find("\"", quoteStart + 1);
                                if (quoteEnd != std::string::npos) {
                                    firstTitle = jsonBody.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Create the payload as a JSON string
        std::string payload = "{";
        payload += "\"json\": " + json::serialize(jsonBody) + ",";
        payload += "\"news_group_name\": " + json::serialize(firstTitle);
        payload += "}";
        
        if (auto res = client.Post(API_URL.c_str(), headers, payload, "application/json")) {
            if (res->status == 200 || res->status == 201) {
                log.updateMessage("Call to Supabase successful\n", LOG_LEVEL::SUCCESS);
                log.log();
                return 1;
            } else {
                throw std::runtime_error("Supabase request failed with status: " + std::to_string(res->status) + 
                                       ", body: " + res->body);
            }
        } else {
            throw std::runtime_error("No response from Supabase API");
        }
    } catch (const std::exception &err) {
        log.updateMessage(err.what(), LOG_LEVEL::ERROR);
        log.log();
        return -1;
    }
}

int fetchNews(std::string mediastack_api_key,std::string supabase_api_key){
    std::string API_URL = "/v1/news?access_key=" + mediastack_api_key;
    Logger log = Logger("nil",LOG_LEVEL::DEBUG);
    try{
        httplib::Client client(MEDIASTACK_API_URL);
        if(auto res = client.Get(API_URL)){
            std::cout<<res->status<<'\n';
            std::string jsonBody = prettifier.prettify(res->body);
            int i = storeToDB(supabase_api_key,jsonBody);
            log.updateMessage("API Call successful\n",LOG_LEVEL::SUCCESS);
            log.log();
            return 1;
        }else {
            throw std::runtime_error("API request failed with status: " + (res ? std::to_string(res->status) : "no response"));
        }
    }
    catch(const std::exception &err){
        log.updateMessage(err.what(),LOG_LEVEL::ERROR);
        log.log();
        return -1;
    }
}

