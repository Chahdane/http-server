## Under Construction
### Currently working on:

- Moving from io/multiplexing logic to a multithreaded approach: Instead of using a single-threaded event loop, you can create a pool of threads to handle incoming connections. This will improve concurrency and allow your web server to handle multiple requests simultaneously.
- Reorganize project structure for better organization and maintainability.
- Implementing a log file
- Enhanceing error handling mechanisms to handle unexpected situations and provide meaningful error messages to clients.
- Expanding CGI Functionality to provide more useful information to CGI scripts, such as the client's IP address, request headers, and server configurations.
- Adding support additional CGI languages: Extend the support for various scripting languages like Python, Ruby, or Node.js...
- Security enhancements: Implementig security measures to prevent common CGI vulnerabilities, such as restricting access to sensitive files and directories and sanitizing user input.
- Adding More Commands to allow user to customize server settings without modifying the source code.
- SSL/TLS support: Adding commands to enable SSL/TLS encryption, allowing secure connections over HTTPS.
- URL rewriting: Add support for URL rewriting rules to provide cleaner and more user-friendly URLs for specific routes or resources.
- Load balancing: Integrating load balancing techniques to distribute incoming requests across multiple server instances for better performance and reliability.
- Implementing support for compressing responses to reduce bandwidth usage and improve loading times for clients.
