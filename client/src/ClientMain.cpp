#include "Atlas.hpp"
#include "core/ClientApplication.hpp"

int main(int argc, char** argv) {

    auto* application = new ClientApplication();
    application->run();
    delete application;
    return 0;
}
