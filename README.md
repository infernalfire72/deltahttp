# deltahttp
C++ HTTP Router with Windows and Linux support

Example:

  ```cpp
  #define PORT 3000
  
  #include <Router.h>
  #include <Methods.h>
  
  using namespace DeltaHttp;
  
  void ExampleFunction(const Request& req, Response& res) {
    res.Write("Hello World!");
  }
  
  int main() {
    Router r;
    
    // Use declared functions
    r.HandleRoute("/",
      ExampleFunction);
      
    // Use lambda function
    r.HandleRoute("/lambda",
      [](const Request& req, Response& res) {
        res.Write("Hello World!");
      });
    
    // Restrict to certain methods
    r.HandleRoute("/methods",
      ExampleFunction,
      HttpMethod::GET | HttpMethod::POST
    );
    
    // Blocking call
    r.Listen(PORT);
  }
  ```
  
  # Multithreading
  really creative
  ```cpp
  #define PORT 3000
  
  #include <Router.h>
  
  using namespace DeltaHttp;
  
  ...
  int main() {
    Router r;
    ...
    
    // Start with 4 Threads
    (r * 4).Listen(PORT);
  }
  ```
  
  # Using your own Response class
  ```cpp
  ...
  #include <Response.h>
  
  class MyResponse : public Response {
    // Writes an Integer as a readable string to the response
    void WriteInt(const int& v) {
      Write(std::to_string(v));
    }
  }
  
  void ExampleFunction(const Request& req, MyResponse& res) {
    res.Write(1234);
  }
  
  ...
  
  int main() {
  ...
  
  r.HandleRoute("/",
    (void (*)(const Request&, Response&))ExampleFunction); // cast function to type void (*)(const Request&, Response&)
  
  ... 
  }
  ```
  
   Tip: Define a macro if you need this a lot
   
   Macro Example: `#define response_cast(func) (void (*)(const Request&, Response&))func`
  
