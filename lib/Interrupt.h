#ifndef SUIKA_INTERRUPT_H
#define SUIKA_INTERRUPT_H

#include <pthread.h>
#include <csignal>
#include <thread>
#include <cstring>
#include <vector>
#include "Logger.h"
#include "EtherDevice.h"
#include "Ipv4.h"
#include "Protocol.h"

namespace suika::interrupt {
    struct Interrupt {
        std::thread::id tid;
        pthread_barrier_t barrier;
        sigset_t sigmask;
        std::unique_ptr<std::thread> interruptThreadPtr;

        std::vector<std::shared_ptr<suika::device::Device> > inputDevices{};

        Interrupt() {
            tid = std::this_thread::get_id();
        }

        void init() {
            pthread_barrier_init(&barrier, nullptr, 2);
            sigemptyset(&sigmask);
            sigaddset(&sigmask, SIGHUP);
            sigaddset(&sigmask, SIGUSR1);
            sigaddset(&sigmask, SIGUSR2);
        }

        void addDevice(const std::shared_ptr<suika::device::Device> &p) {
            sigaddset(&sigmask, p->getIrq());
            inputDevices.emplace_back(p);
        }

        void run() {
            //std::vector<std::string> devicesStr;
            //std::transform(inputDevices.begin(), inputDevices.end(), std::back_inserter(devicesStr),
            //               [](const std::shared_ptr<suika::device::Device>& protocolDataPtr) { return std::format("{}", protocolDataPtr->getIrq()); });

            suika::logger::info("run interrupt handle thread.");
            suika::logger::info("registered devices:");
            for (const auto &p: inputDevices) {
                suika::logger::info(std::format("- {}", p->getInfo()));
            }

            int err;
            err = pthread_sigmask(SIG_BLOCK, &sigmask, nullptr);
            if (err) {
                auto msg = std::format("erropthread_sigmask {}", strerror(err));
                suika::logger::error(msg);
                throw std::runtime_error(msg);
            }
            suika::logger::info("create interrupt thread");
            interruptThreadPtr = std::make_unique<std::thread>(&Interrupt::interruptThread, this);
            pthread_barrier_wait(&barrier);
        }

        void shutdown() {
            suika::logger::info("call thread shutdown");
            pthread_kill(interruptThreadPtr->native_handle(), SIGHUP);
            interruptThreadPtr->join();
            suika::logger::info("finish thread shutdown");
        }

    private:
        void interruptThread() {
            suika::logger::info("run interrupt thread");
            int terminate = 0;

            pthread_barrier_wait(&barrier);
            while (!terminate) {
                int sig;
                int err = sigwait(&sigmask, &sig);
                if (err) {
                    suika::logger::error(std::format("sigwait() {}", strerror(err)));
                }
                switch (sig) {
                    case SIGHUP:
                        terminate = 1;
                        suika::logger::info("receive SIGHUP exit interrupt loop");
                        break;
                    case SIGUSR1:
                        suika::logger::info("receive SIGUSR1");
                        handleProtocolSignal();
                        break;
                    case SIGUSR2:
                        suika::logger::info("receive SIGUSR2");
                        handleEventSignal();
                        break;
                    default:
                        suika::logger::info(std::format("signal = {}", sig));
                        handleSignal(sig);
                        break;
                }
            }
        }

        void handleProtocolSignal() {
            std::lock_guard<std::mutex> lock(suika::protocol::protocolQueuesMutex);

            for (auto &protocolQueue: suika::protocol::protocolQueues) {
                auto type = protocolQueue.first;
                suika::logger::info(std::format("check protocol queue: type = {:04x}, length = {}",
                                                type, protocolQueue.second.size()));
                while (!protocolQueue.second.empty()) {
                    auto d = protocolQueue.second.front();
                    protocolQueue.second.pop();
                    suika::protocol::protocolHandlers[type]->handle(d);
                }
            }
        }

        int handleEventSignal() {
            for (auto& e : suika::protocol::ipv4::eventHandlers) {
                e->handle();
            }
            return 0;
        }

        void handleSignal(int sig) {
            for (const auto &d: inputDevices) {
                if (d->getIrq() == sig) {
                    suika::logger::info(std::format("device found. process payload. device = {}", d->getInfo()));
                    d->handler(interruptThreadPtr->native_handle());
                }
            }
        }
    };
}

#endif //SUIKA_INTERRUPT_H
