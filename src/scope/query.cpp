#include <boost/algorithm/string/trim.hpp>

#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

#include <iomanip>
#include <sstream>

namespace sc = unity::scopes;
namespace alg = boost::algorithm;

using namespace std;
using namespace api;
using namespace scope;


/**
 * Define the layout for the forecast results
 *
 * The icon size is small, and ask for the card layout
 * itself to be horizontal. I.e. the text will be placed
 * next to the image.
 */
const static string RESULT_TEMPLATE =
        R"(
{
        "schema-version": 1,
        "template": {
        "category-layout": "grid",
        "card-layout": "vertical",
        "card-size": "large"
        },
        "components": {
        "title": "title",
        "art" : {
        "field": "art"
        },
        "subtitle": "subtitle",
        "summary": "summary"
        }
        }
        )";

/**
 * Define the larger "current weather" layout.
 *
 * The icons are larger.
 */
const static string CITY_TEMPLATE =
        R"(
{
        "schema-version": 1,
        "template": {
        "category-layout": "grid",
        "card-size": "medium"
        },
        "components": {
        "title": "title",
        "art" : {
        "field": "art"
        },
        "subtitle": "subtitle"
        }
        }
        )";

Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata,
             Config::Ptr config) :
    sc::SearchQueryBase(query, metadata), client_(config) {
}

void Query::cancelled() {
    client_.cancel();
}


void Query::run(sc::SearchReplyProxy const& reply) {
    try {
        // Start by getting information about the query
        const sc::CannedQuery &query(sc::SearchQueryBase::query());

        // Trim the query string of whitespace
        string query_string = alg::trim_copy(query.query_string());

        // the Client is the helper class that provides the results
        // without mixing APIs and scopes code.
        // Add your code to retreive xml, json, or any other kind of result
        // in the client.
        Client::QueryResult result;
        bool show_welcome = false;

        if (query_string.empty()) {
            // If the string is empty, get the current weather for London
            //result = client_.query("www.omgubuntu.co.uk/2015/06/ios-9-siri-suggestions-ubuntu-scopes");
            show_welcome = true;
        } else {
            // otherwise, get the current weather for the search string
            result = client_.query(query_string);
        }

        // Register a category for the forecast
        auto result_cat = reply->register_category("res", "", "", sc::CategoryRenderer(RESULT_TEMPLATE));

        sc::CategorisedResult res(result_cat);
        if(show_welcome) {
            res.set_uri("welcome");
            res.set_title("Welcome in Sherlock!");
            res.set_art("http://marioguerrieroesempio.altervista.org/data/happy-face.png");
            res["summary"] = "I couldn't find any result. Please, change you query or check your connectivity and try again.";
            res["description"] = "No results found";
            res["type"] = "empty";
        }
        /**
          * 404 Error
          */
        else if(result.error) {
            res.set_uri("-1");
            res.set_title("Nothing here");
            res.set_art("http://marioguerrieroesempio.altervista.org/data/sad-face.png");
            res["summary"] = "I couldn't find any result. Please, change you query or check your connectivity and try again.";
            res["description"] = "No results found";
            res["type"] = "empty";
        }
        /**
          * Something was actually found
          */
        else {
            res.set_uri(result.result.link);
            res.set_title(result.result.title);
            res.set_art(result.result.image);
            res["subtitle"] = result.result.author;
            res["original"] = result.result.article;
            res["summary"] = result.result.tags;
            res["description"] = result.result.summary;
        }

        // Push the result
        if (!reply->push(res)) {
            // If we fail to push, it means the query has been cancelled.
            // So don't continue;
            return;
        }

    } catch (domain_error &e) {
        // Handle exceptions being thrown by the client API
        cerr << e.what() << endl;
        reply->error(current_exception());
    }
}
