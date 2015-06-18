#include <api/client.h>

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <QVariantMap>
#include <QUrl>
#include <QString>

#include <iostream>
#include <string>
#include <algorithm>
#include <QMapIterator>

namespace http = core::net::http;
namespace net = core::net;

using namespace api;
using namespace std;

Client::Client(Config::Ptr config) :
    config_(config), cancelled_(false) {
}


void Client::get(const net::Uri::Path &path,
                 const net::Uri::QueryParameters &parameters, QJsonDocument &root) {
    // Create a new HTTP client
    auto client = http::make_client();

    // Start building the request configuration
    http::Request::Configuration configuration;

    // Build the URI from its components
    net::Uri uri = net::make_uri(config_->apiroot, path, parameters);
    configuration.uri = client->uri_to_string(uri);

    // Give out headers
    configuration.header.add("User-Agent", config_->user_agent);
    configuration.header.add("X-Mashape-Key", config_->key);
    configuration.header.add("Accept", "application/json");

    // Build a HTTP request object from our configuration
    auto request = client->head(configuration);

    try {
        // Synchronously make the HTTP request
        // We bind the cancellable callback to #progress_report
        auto response = request->execute(
                    bind(&Client::progress_report, this, placeholders::_1));

        // Check that we got a sensible HTTP status code
        if (response.status != http::Status::ok) {
            throw domain_error(response.body);
        }
        // Parse the JSON from the response
        root = QJsonDocument::fromJson(response.body.c_str());

        // Open weather map API error code can either be a string or int
        QVariant cod = root.toVariant().toMap()["cod"];
        if ((cod.canConvert<QString>() && cod.toString() != "200")
                || (cod.canConvert<unsigned int>() && cod.toUInt() != 200)) {
            throw domain_error(root.toVariant().toMap()["message"].toString().toStdString());
        }
    } catch (net::Error &) {
    }
}

Client::QueryResult Client::query(const string &query)
{
    QJsonDocument root;

    // Build a URI and get the contents.
    // The fist parameter forms the path part of the URI.
    // The second parameter forms the CGI parameters.
    QByteArray text = QByteArray::fromStdString(query);
    QByteArray encoded(text.toPercentEncoding());
    std::string par = "False"; par += "&link="; par += + encoded.data();
    get(
    { "link" },
    { { "entity_description", "False" }, { "link", encoded.data() } },
                root);

    QueryResult result;

    QVariantMap variant = root.toVariant().toMap();

    // Check if error occurred
    //result.error = variant["errors"].isNull();
    //cout << "yee" << endl;
    //if(result.error)
    //    return result;
    result.error = false;

    // Build result if no error occurred
    QVariantList summaries = variant["summary"].toList();
    if(summaries.isEmpty()) {
        result.error = true;
        return result;
    }
    std::string summary = "";
    for (const QVariant &i : summaries) {
        summary += i.toString().toStdString() + "\n";
    }
    QVariantList tags = variant["tags"].toList();
    std::string tag = "";
    for (const QVariant &i : tags) {
        std::string str = i.toString().toStdString();
        //std::replace(str.begin(), str.end(), ' ', '');
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        tag += "#" + str + " ";
    }
    result.result = Result {
            variant["title"].toString().toStdString(),
            variant["author"].toString().toStdString(),
            variant["image"].toString().toStdString(),
            variant["language"].toString().toStdString(),
            variant["link"].toString().toStdString(),
            variant["cleaned_text"].toString().toStdString(),
            summary,
            tag
    };

return result;
}

http::Request::Progress::Next Client::progress_report(
        const http::Request::Progress&) {

    return cancelled_ ?
                http::Request::Progress::Next::abort_operation :
                http::Request::Progress::Next::continue_operation;
}

void Client::cancel() {
    cancelled_ = true;
}

Config::Ptr Client::config() {
    return config_;
}

