#include <vix.hpp>
#include "src/utils.hpp"

int main(void) {
    int fetchDailyNews = fetchNews();
    if(fetchDailyNews < 0){
        return -1;
    }
    std::cout<<"Succesfully fetched news\n";
}