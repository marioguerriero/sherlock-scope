#ifndef API_CONFIG_H_
#define API_CONFIG_H_

#include <memory>
#include <string>

namespace api {

struct Config {
    typedef std::shared_ptr<Config> Ptr;

    const std::string key { "YxgUJYyNOYmshr22SJXblBHkXNZ7p1BlnJCjsnYlEadjiIJ3f7" };

    /*
     * The root of all API request URLs
     */
    std::string apiroot { "https://joanfihu-article-analysis-v1.p.mashape.com" };

    /*
     * The custom HTTP user agent string for this library
     */
    std::string user_agent { "example-network-scope 0.1; (foo)" };
};

}

#endif /* API_CONFIG_H_ */

