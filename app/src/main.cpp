#include "error_handler.hpp"
#include <iostream>

using namespace std;

enum Error {
    MY_ERROR_ONE,
    MY_ERROR_TWO,
    MY_ERROR_THREE
};

vector<tuple<Error, string>> error_messages = {
    {MY_ERROR_ONE, "Error One message"},
    {MY_ERROR_TWO, "Error Two message"},
    {MY_ERROR_THREE, "Error Three message"}
};

Error error_handler_cb(tuple<Error, string> handler) {
    cout << get<string>(handler) << endl;
    return get<Error>(handler);
}

template class Callback<tuple<Error, string>, Error>;
template class CallbackManager<tuple<Error, string>, Error>;
template class ConfigErrorManager<tuple<Error, string>, Error>;

int main() {

    ConfigErrorManager<tuple<Error, string>, Error> error_handler;
    error_handler.registerErrorMessages(error_messages);
    error_handler.registerErrorCallback(Error::MY_ERROR_ONE, error_handler_cb);
    error_handler.registerErrorCallback(Error::MY_ERROR_TWO, error_handler_cb);
    error_handler.registerErrorCallback(Error::MY_ERROR_THREE, error_handler_cb);
    auto error_message = error_handler.getErrorMessage(Error::MY_ERROR_ONE);
    tuple <Error, string> handler_cb_parameter = make_tuple(Error::MY_ERROR_ONE, error_message.value());
    auto err = error_handler.handleError(Error::MY_ERROR_ONE, handler_cb_parameter);
    return 0;
}