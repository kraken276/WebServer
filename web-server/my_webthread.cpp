#include "my_headers.h"
#include <thread>

using namespace std;

namespace my_namespace {
    void startthread(my_webthread* thread) {
        my_pointer<my_webthread> pointer(thread);
        my_webserver* server = thread->server;
        thread->func(thread->request, thread->socket);
        server->freethreadsmutex.lock();
        server->freethreads++;
        server->freethreadsmutex.unlock();
    }
    my_webthread::my_webthread() {
        server = NULL;
        func = NULL;
        socket = INVALID_SOCKET;
    };
    void my_webthread::checkthreadsqueue() {
        server->queuethreadsmutex.lock();
        int queuethreadscount = server->threads.size();
        if (queuethreadscount > 0) {
            auto thread = server->threads.front();
            server->threads.pop_front();
            thread->trystart();
        }
        server->queuethreadsmutex.unlock();
    }
    void my_webthread::start() {
        server->freethreadsmutex.lock();
        server->freethreads--;
        server->freethreadsmutex.unlock();
        thread requestthread(startthread, this);
        requestthread.detach();
    }
    void my_webthread::trystart() {
        server->freethreadsmutex.lock();
        int freethreads = server->freethreads;
        server->freethreadsmutex.unlock();
        if (freethreads == 0)
            server->threads.push_back(this);
        else
            start();
    }
    my_webthread::~my_webthread() {
        shutdown(socket, SD_SEND);
        closesocket(socket);
        checkthreadsqueue();
    };
}