#pragma once
#include <sstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cctype>
struct HttpRequest {
    std::string method;
    std::string path;
    std::string http_version;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> body;

    HttpRequest() = default;

    explicit HttpRequest(const std::string& raw_request) {
        std::istringstream stream(raw_request);
        std::string line;

    
        if (std::getline(stream, line)) {
            std::istringstream line_stream(line);
            line_stream >> method >> path >> http_version;
        }

       
        while (std::getline(stream, line) && line != "\r" && !line.empty()) {
            auto colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);
                key.erase(key.find_last_not_of(" \r\n") + 1);
                value.erase(0, value.find_first_not_of(" "));
                value.erase(value.find_last_not_of("\r\n") + 1);
                headers[key] = value;
            }
        }

      
        std::ostringstream body_stream;
        while (std::getline(stream, line)) {
            body_stream << line << "\n";
        }
        std::string raw_body = body_stream.str();
        if (!raw_body.empty() && raw_body.back() == '\n') raw_body.pop_back();

      
     auto it = headers.find("Content-Type");
if (it != headers.end() && it->second.find("application/json") != std::string::npos) {
    parseJsonBody(raw_body);
}
    }

private:
void parseJsonBody(const std::string& json) {
    try {
        size_t pos = 0;
        while (pos < json.size()) {
            pos = json.find('"', pos);
            if (pos == std::string::npos) break;
            size_t key_start = pos + 1;
            size_t key_end = json.find('"', key_start);
            if (key_end == std::string::npos) break;
            std::string key = json.substr(key_start, key_end - key_start);

            pos = json.find(':', key_end);
            if (pos == std::string::npos) break;
            ++pos;

            while (pos < json.size() && std::isspace(json[pos])) ++pos;
            if (pos >= json.size()) break;

            std::string value;
            if (json[pos] == '"') {
                size_t value_start = pos + 1;
                size_t value_end = json.find('"', value_start);
                if (value_end == std::string::npos) break;
                value = json.substr(value_start, value_end - value_start);
                pos = value_end + 1;
            } else {
                size_t value_start = pos;
                while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-' || json[pos] == '.')) ++pos;
                value = json.substr(value_start, pos - value_start);
            }

            body[key] = value;

            pos = json.find(',', pos);
            if (pos == std::string::npos) break;
            ++pos;
        }
    } catch (const std::exception& e) {
        std::cerr << "[parseJsonBody] Error parsing JSON: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[parseJsonBody] Unknown error occurred while parsing JSON.\n";
    }
}


};
struct HttpResponse {
    std::string http_version = "HTTP/1.1";
    int status_code = 200;
    std::string status_message = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;  

   
    void json(const std::string& json_str) {
        body = json_str;
        headers["Content-Type"] = "application/json";
        headers["Content-Length"] = std::to_string(body.size());
    }

    std::string toString() const {
        std::string resp = http_version + " " + std::to_string(status_code) + " " + status_message + "\r\n";
        for (const auto& h : headers) {
            resp += h.first + ": " + h.second + "\r\n";
        }
        resp += "\r\n";
        resp += body;
        return resp;
    }
};
