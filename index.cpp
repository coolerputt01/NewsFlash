#include "src/utils.hpp"
#include "include/env/dot_env.h"
#include <cstdlib>
#include <tgbot/tgbot.h>

int main(void) {
    env_load(".env",0);
    auto require_env = [](const char* name) -> std::string {
        const char* val = std::getenv(name);
        if (!val) {
            throw std::runtime_error(std::string("Missing env var: ") + name);
        }
        return std::string(val);
    };

    
    std::string supabase_api_url = require_env("SUPABASE_SECRET_KEY");
    std::string telegram_token = require_env("TELEGRAM_BOT_TOKEN");
    std::string chat_id = require_env("TELEGRAM_CHAT_ID");
    std::string media_stack_api_url = require_env("MEDIA_STACK_ACCESS_KEY");
    TgBot::Bot bot(telegram_token);
    
    // int fetchDailyNews = fetchNews(media_stack_api_url,supabase_api_url);
    // if(fetchDailyNews < 0){
    //     return -1;
    // }
    initialiseBot(bot,media_stack_api_url,supabase_api_url,chat_id);

    std::cout<<"Succesfully fetched news\n";
}