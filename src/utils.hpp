#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma once

#include <string>
#include <stdexcept>
#include <tgbot/tgbot.h>

#include "../include/httpreq/httplib.h"
#include "../include/logger/loggand.hpp"
#include "../include/json/nholmann.hpp"
#include "../config.hpp"
#include "./newsModel.hpp"


using json = nlohmann::json;

json fetchJsonBatch(std::string api_key){
    std::string API_URL = "/rest/v1/news";
    Logger log = Logger("nil", LOG_LEVEL::DEBUG);
    
    try {
        httplib::Client client(SUPABASE_API_URL);
        
        httplib::Headers headers = {
            {"apikey", api_key},
            {"Authorization", "Bearer " + api_key},
            {"Content-Type", "application/json"}
        };
        
        if (auto res = client.Get(API_URL.c_str(), headers)) {
            if (res->status == 200) {
                json responseJson = json::parse(res->body);
                
                log.updateMessage("Successfully fetched JSON batch from Supabase", LOG_LEVEL::SUCCESS);
                log.log();                
                return responseJson;
            } else {
                throw std::runtime_error("Supabase request failed with status: " + std::to_string(res->status) + ", body: " + res->body);
            }
        } else {
            throw std::runtime_error("No response from Supabase API");
        }
        
    } catch (const json::parse_error &err) {
        log.updateMessage(std::string("JSON parse error: ") + err.what(), LOG_LEVEL::ERROR);
        log.log();
        throw;
    } catch (const std::exception &err) {
        log.updateMessage(err.what(), LOG_LEVEL::ERROR);
        log.log();
        throw;
    }
    
    return json();
}

bool sendBatchMessages(TgBot::Bot &bot, std::string apikey, std::string chatID) {
    try {
        json parsedJson = fetchJsonBatch(apikey);

        if (!parsedJson.is_array() || parsedJson.empty()) {
            bot.getApi().sendMessage(chatID, "No news alerts available at this time.");
            return true;
        }

        // Get latest (ensure your DB query orders this correctly)
        json latestNewsGroup = parsedJson.back();

        if (!latestNewsGroup.contains("json") || !latestNewsGroup["json"].is_object()) {
            bot.getApi().sendMessage(chatID, "Invalid news data format.");
            return true;
        }

        json newsData = latestNewsGroup["json"];

        if (!newsData.contains("data") || !newsData["data"].is_array()) {
            bot.getApi().sendMessage(chatID, "No news items in the latest batch.");
            return true;
        }

        json newsAlerts = newsData["data"];

        if (newsAlerts.empty()) {
            bot.getApi().sendMessage(chatID, "No news items in the latest batch.");
            return true;
        }

        auto linkPreviewOptions = std::make_shared<TgBot::LinkPreviewOptions>();
        linkPreviewOptions->isDisabled = true;

        int messageCount = 0;

        for (auto& news : newsAlerts) {
            std::string messageText;

            // Title
            if (news.contains("title") && news["title"].is_string()) {
                messageText += "📰 *" + news["title"].get<std::string>() + "*\n\n";
            }

            // Description
            if (news.contains("description") && news["description"].is_string()) {
                std::string desc = news["description"].get<std::string>();
                if (!desc.empty()) {
                    if (desc.length() > 500) {
                        desc = desc.substr(0, 497) + "...";
                    }
                    messageText += desc + "\n\n";
                }
            }

            // URL
            if (news.contains("url") && news["url"].is_string()) {
                messageText += "🔗 [Read more](" + news["url"].get<std::string>() + ")\n";
            }

            // Date
            if (news.contains("published_at") && news["published_at"].is_string()) {
                messageText += "📅 " + news["published_at"].get<std::string>();
            }

            if (messageText.empty()) continue;

            try {
                bot.getApi().sendMessage(chatID, messageText, linkPreviewOptions, nullptr, nullptr, "Markdown", false);
                messageCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            } catch (const TgBot::TgException&) {
                // Fallback: plain text (strip markdown)
                std::string plainText = messageText;

                // Remove markdown symbols
                plainText.erase(std::remove(plainText.begin(), plainText.end(), '*'), plainText.end());

                // Replace markdown links [text](url) → text (url)
                size_t pos = 0;
                while ((pos = plainText.find("[", pos)) != std::string::npos) {
                    size_t end = plainText.find("](", pos);
                    if (end != std::string::npos) {
                        size_t urlEnd = plainText.find(")", end);
                        if (urlEnd != std::string::npos) {
                            std::string text = plainText.substr(pos + 1, end - pos - 1);
                            std::string url = plainText.substr(end + 2, urlEnd - end - 2);
                            plainText.replace(pos, urlEnd - pos + 1, text + " (" + url + ")");
                            pos += text.length();
                        } else break;
                    } else break;
                }

                bot.getApi().sendMessage(chatID, plainText);
                messageCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // Summary
        if (messageCount > 0) {
            bot.getApi().sendMessage(chatID, "✅ Sent " + std::to_string(messageCount) + " news updates!");
        } else {
            bot.getApi().sendMessage(chatID, "No valid news items to display.");
        }

        return true;

    } catch (const std::exception& e) {
        bot.getApi().sendMessage(chatID, "❌ Error: " + std::string(e.what()));
        return false;
    }
}

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

bool initialiseBot(auto &bot, std::string apikey, std::string chatID){
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

    bot.getEvents().onCommand("news", [&bot,&apikey,&chatID](TgBot::Message::Ptr message) {
        std::string chatId = std::to_string(message->chat->id);
        bot.getApi().sendMessage(message->chat->id, "Fetching latest news...");
        sendBatchMessages(bot, apikey, chatID);
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

