#include <vix/requests/requests>
#include <vix/utils/Env.hpp>
#include <vix/utils/Logger.hpp>
#include "../config.hpp"

auto &log = vix::utils::Logger::getInstance();
log.setLevel(vix::utils::Logger::Level::Debug);

const std::string MEDIASTACK_ACCESS_KEY = vix::utils::env_or("MEDIA_STACK_ACCESS_KEY","nil");

int fetchNews(){
    std::string API_URL = MEDIASTACK_API_URL + "?access_key=" + MEDIASTACK_ACCESS_KEY;
    try{
        auto res = vix::requests::get(API_URL);
        std::string struc_res = "Response - "+res.status_code +'\n'+res.text+'\n';
        log.info(struc_res);
        if(!response.ok()){
            throw;
        }
    }
    catch(const std::exception &err){
        log.error(e.what());
        return -1;
    }
}