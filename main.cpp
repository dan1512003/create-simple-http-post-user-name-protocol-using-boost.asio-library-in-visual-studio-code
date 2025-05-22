#include "Router/router.hpp"
#include "app/app.hpp"

struct User {
    int id;
    std::string name;
};
void handlePostUser(HttpRequest& req, HttpResponse& res) {
    try {
        User user;

        if (req.body.find("id") != req.body.end()) {
            user.id = std::stoi(req.body["id"]);
        } else {
            res.status_code = 400;
            res.status_message = "Bad Request";
            res.json(R"({"error":"Missing 'id'"})");
            return;
        }

        if (req.body.find("name") != req.body.end()) {
            user.name = req.body["name"];
        } else {
            res.status_code = 400;
            res.status_message = "Bad Request";
            res.json(R"({"error":"Missing 'name'"})");
            return;
        }

        std::cout << "Received user: id=" << user.id << ", name=" << user.name << "\n";

        res.status_code = 201;
        res.status_message = "Created";
     
        res.json(
            "{\"message\":\"User created successfully\",\"user\":{\"id\":" + 
            std::to_string(user.id) + ",\"name\":\"" + 
            user.name + "\"}}");

    } catch (const std::exception& e) {
        res.status_code = 500;
        res.status_message = "Internal Server Error";
        res.json(std::string("{\"error\":\"") + e.what() + "\"}");
    }
}

int main() {
 app server;
    Router router;

    router.post("/user",handlePostUser );
    server.use("/admin", router);

    server.listen(8080);
    return 0;
}
