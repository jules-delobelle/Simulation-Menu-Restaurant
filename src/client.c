#include <stdio.h>


int main() {
    
    while(1){
        read(pip_request[0], request, sizeof(request));

        traiter(request);

        write(pip_response[1], response, sizeof(response));
    }

    return 0;
}