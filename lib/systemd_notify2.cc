// Copyright (c) 2018 ExcitableAardvark <excitableaardvark@tutanota.de>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <assert.h>
#include <iostream>

#include <node_api.h>

extern "C" {
	#include <systemd/sd-daemon.h>
}

napi_value Notify(napi_env env, napi_callback_info info) {
	napi_status status;

	size_t argc = 1;
	napi_value args[1];
	status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
	assert(status == napi_ok);

	if (argc != 1) {
		napi_throw_type_error(env, nullptr, "Incorrect arguments");
		return nullptr;
	}

	napi_valuetype valuetype;
	status = napi_typeof(env, args[0], &valuetype);
	assert(status == napi_ok);

	if (valuetype != napi_string) {
		napi_throw_type_error(env, nullptr, "Incorrect arguments");
		return nullptr;
	}

	size_t length;
	status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &length);
	assert(status == napi_ok);

	char *param = new char[length];
	assert(param != nullptr);
	status = napi_get_value_string_utf8(env, args[0], param, length + 1, nullptr);
	assert(status == napi_ok);

	sd_notify(0, param);

	delete param;

	return nullptr;
}

napi_value Init(napi_env env, napi_value exports) {
	napi_status status;

	napi_property_descriptor notifyDesc =
		{ "notify", 0, Notify, 0, 0, 0, napi_default, 0 };

	status = napi_define_properties(env, exports, 1, &notifyDesc);
	assert(status == napi_ok);

	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
