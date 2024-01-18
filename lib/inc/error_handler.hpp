#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <unordered_map>
#include <type_traits>
#include <optional>
#include <variant>
#include <functional>
#include <boost/functional/hash.hpp>

namespace std {

    template <class T, typename E>
    struct Callback {
        function<E(T)> callback;
    };

    template <class T, typename E>
    class CallbackManager {
        
        private:

            std::unordered_map<size_t, unique_ptr<Callback<T,E>>> callbacks_;
            
            size_t generateUniqueIdentifier(E error, const type_info& typeInfo) {
                size_t seed = 0;
                boost::hash_combine(seed, error);
                boost::hash_combine(seed, typeInfo.hash_code());
                return seed;
            }

        public:

            CallbackManager() {}

            void registerCallback(E error, function<E(T)> callback) {
                size_t key = this->generateUniqueIdentifier(error, typeid(T));
                this->callbacks_[key] = make_unique<Callback<T,E>>(Callback<T,E>{callback});
            }

            optional<E> invokeCallback(E error, T value) {
                size_t key = this->generateUniqueIdentifier(error, typeid(T));
                auto it = callbacks_.find(key);
                if (it != callbacks_.end()) {
                    auto callback = dynamic_cast<Callback<T,E>*>(it->second.get());
                    if (callback) {
                        return callback->callback(value);
                    }
                } 

                return nullopt;
            }

    };

    template <class T, typename E>
    class ConfigErrorManager {

        private:
        
            unordered_map<E, string> messages;
            CallbackManager<T,E> handlers;

        public:

            ConfigErrorManager() {}

            void registerErrorMessages(vector<tuple<E, string>> error_messages) {
                for (const auto& error_message : error_messages) {
                    auto [error, message] = error_message;
                    this->messages[error] = message;
                }
            }

            void registerErrorCallback(E error, function<E(T)> error_callback) {
                this->handlers.registerCallback(error, error_callback);
            }

            optional<string> getErrorMessage(E error) {
                if(this->messages.find(error) != this->messages.end()) {
                    return this->messages[error];
                } else {
                    return nullopt;
                }
            }

            optional<E> handleError(E error, T parameter) {
                return this->handlers.invokeCallback(error, parameter);
            }
            
    };

}


#endif