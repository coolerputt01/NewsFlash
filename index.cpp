#include "src/utils.hpp"
#include "include/env/dot_env.h"
#include <cstdlib>
#include <tgbot/tgbot.h>

int main(void) {
    env_load(".env",0);
    const char* media_stack_api_url = std::getenv("MEDIA_STACK_ACCESS_KEY");
    const char* supabase_api_url = std::getenv("SUPABASE_SECRET_KEY");
    const char* TELEGRAM_BOT_API_TOKEN = std::getenv("TELEGRAM_BOT_TOKEN");
    const char* TELEGRAM_BOT_CHAT_ID = std::getenv("TELEGRAM_CHAT_ID");
    TgBot::Bot bot(TELEGRAM_BOT_API_TOKEN);
    
    // int fetchDailyNews = fetchNews(media_stack_api_url,supabase_api_url);
    // if(fetchDailyNews < 0){
    //     return -1;
    // }
    initialiseBot(bot,supabase_api_url,TELEGRAM_BOT_CHAT_ID);

    std::cout<<"Succesfully fetched news\n";
}