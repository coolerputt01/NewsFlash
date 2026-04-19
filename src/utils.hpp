#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma once

#include <string>
#include <stdexcept>
#include "../include/httpreq/httplib.h"
#include "../include/logger/loggand.hpp"
#include "../include/json/nholmann.hpp"
#include "../config.hpp"
#include "./newsModel.hpp"

using json = nlohmann::json;

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
        
        json parsedJson = json::parse(jsonBody);
        
        std::string firstTitle;
        if (parsedJson.contains("data") && parsedJson["data"].is_array() && !parsedJson["data"].empty()) {
            if (parsedJson["data"][0].contains("title")) {
                firstTitle = parsedJson["data"][0]["title"].get<std::string>();
            }
        }
        
        json payload;
        payload["json"] = parsedJson;
        payload["news_group_name"] = firstTitle;
        
        std::string payloadStr = payload.dump();
        
        if (auto res = client.Post(API_URL.c_str(), headers, payloadStr, "application/json")) {
            if (res->status == 200 || res->status == 201) {
                log.updateMessage("Call to Supabase successful\n", LOG_LEVEL::SUCCESS);
                log.log();
                return 1;
            } else {
                throw std::runtime_error("Supabase request failed with status: " + std::to_string(res->status) + ", body: " + res->body);
            }
        } else {
            throw std::runtime_error("No response from Supabase API");
        }
    } catch (const json::parse_error &err) {
        log.updateMessage(std::string("JSON parse error: ") + err.what(), LOG_LEVEL::ERROR);
        log.log();
        return -1;
    } catch (const std::exception &err) {
        log.updateMessage(err.what(), LOG_LEVEL::ERROR);
        log.log();
        return -1;
    }
}

int fetchNews(std::string mediastack_api_key, std::string supabase_api_key){
    std::string API_URL = "/v1/news?access_key=" + mediastack_api_key;
    Logger log = Logger("nil", LOG_LEVEL::DEBUG);
    try{
        httplib::Client client(MEDIASTACK_API_URL);
        if(auto res = client.Get(API_URL)){
            std::cout << res->status << '\n';
            
            json jsonResponse = json::parse(res->body);
            std::string jsonBody = jsonResponse.dump(4);
            
            int i = storeToDB(supabase_api_key, jsonBody);
            log.updateMessage("API Call successful\n", LOG_LEVEL::SUCCESS);
            log.log();
            return 1;
        } else {
            throw std::runtime_error("API request failed with status: " + (res ? std::to_string(res->status) : "no response"));
        }
    }
    catch(const json::parse_error &err){
        log.updateMessage(std::string("JSON parse error in fetchNews: ") + err.what(), LOG_LEVEL::ERROR);
        log.log();
        return -1;
    }
    catch(const std::exception &err){
        log.updateMessage(err.what(), LOG_LEVEL::ERROR);
        log.log();
        return -1;
    }
}

bool initialiseBot(std::string token){
    if (token.empty()) {
        std::cerr<<"Error: TELEGRAM_BOT_API_TOKEN is empty!\n";
        return false;
    }
    TgBot::Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
        return true;
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
        return false;
    }catch (const std::exception& e) {
        printf("Unexpected error: %s\n", e.what());
        return false;
    }
}

