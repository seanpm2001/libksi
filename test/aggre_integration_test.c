/*
 * Copyright 2013-2015 Guardtime, Inc.
 *
 * This file is part of the Guardtime client SDK.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES, CONDITIONS, OR OTHER LICENSES OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 * "Guardtime" and "KSI" are trademarks or registered trademarks of
 * Guardtime, Inc., and no license to trademarks is granted; Guardtime
 * reserves and retains all trademark rights.
 */

#include "cutest/CuTest.h"
#include "all_integration_tests.h"
#include <ksi/net_uri.h>
#include <ksi/net_http.h>
#include <ksi/net.h>

extern KSI_CTX *ctx;


static void Test_NOKAggr_TreeTooLarge(CuTest* tc) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_AggregationReq *request = NULL;
	KSI_RequestHandle *handle = NULL;
	KSI_AggregationResp *response = NULL;

	KSI_DataHash *hsh = NULL;
	KSI_Integer *ID = NULL;
	KSI_Integer *requestLevel = NULL;
	KSI_Integer *resp_status = NULL;

	KSI_ERR_clearErrors(ctx);



	res = KSI_AggregationReq_new(ctx, &request);
	CuAssert(tc, "Unable to create aggregation request.", res == KSI_OK && request != NULL);

	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_Integer_new(ctx, 0x5544332211, &ID);
	CuAssert(tc, "Unable to create request ID.", res == KSI_OK && ID != NULL);

	res = KSI_Integer_new(ctx, 0xffffff, &requestLevel);
	CuAssert(tc, "Unable to create request level.", res == KSI_OK && requestLevel != NULL);

	res = KSI_AggregationReq_setRequestLevel(request, requestLevel);
	CuAssert(tc, "Unable to create request level.", res == KSI_OK);
	requestLevel = NULL;

	res = KSI_AggregationReq_setRequestId(request, ID);
	CuAssert(tc, "Unable set request ID.", res == KSI_OK);
	ID = NULL;

	res = KSI_AggregationReq_setRequestHash(request, hsh);
	CuAssert(tc, "Unable set request hash.", res == KSI_OK);
	hsh = NULL;

	res = KSI_sendSignRequest(ctx, request, &handle);
	CuAssert(tc, "Unable to send (prepare) sign request.", res == KSI_OK);

	res = KSI_RequestHandle_perform(handle);
	CuAssert(tc, "Unable to send (perform) sign request.", res == KSI_OK);

	res = KSI_RequestHandle_getAggregationResponse(handle, &response);
	CuAssert(tc, "Unable to get (send and get) sign request.", res == KSI_OK && response != NULL);

	res = KSI_AggregationResp_getStatus(response, &resp_status);
	CuAssert(tc, "Unable to get response status.", res == KSI_OK && resp_status != NULL);
	CuAssert(tc, "WrongErrorCode.", KSI_Integer_equalsUInt(resp_status, 0x104));

	KSI_AggregationReq_free(request);
	KSI_RequestHandle_free(handle);
	KSI_AggregationResp_free(response);

	KSI_DataHash_free(hsh);
	KSI_Integer_free(requestLevel);
	KSI_Integer_free(ID);
	return;
}

static void Test_CreateSignatureDefProvider(CuTest* tc) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_DataHash *hsh = NULL;
	KSI_Signature *sig = NULL;


	KSI_ERR_clearErrors(ctx);

	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_Signature_create(ctx, hsh, &sig);
	CuAssert(tc, "Unable to create signature.", res == KSI_OK && sig != NULL);

	res = KSI_Signature_verifyDataHash(sig, ctx, hsh);
	CuAssert(tc, "Unable to verify signature.", res == KSI_OK);

	KSI_DataHash_free(hsh);
	KSI_Signature_free(sig);
	return;
}

static void Test_TCPCreateSignatureDefProvider(CuTest* tc) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_DataHash *hsh = NULL;
	KSI_Signature *sig = NULL;


	KSI_ERR_clearErrors(ctx);

	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_CTX_setAggregator(ctx, conf.tcp_url, conf.tcp_user, conf.tcp_pass);
	CuAssert(tc, "Unable to spoil aggregator authentication data.", res == KSI_OK);

	res = KSI_Signature_create(ctx, hsh, &sig);
	KSI_CTX_setAggregator(ctx, conf.aggregator_url, conf.aggregator_user, conf.aggregator_pass);
	CuAssert(tc, "Unable to create signature.", res == KSI_OK && sig != NULL);

	res = KSI_Signature_verifyDataHash(sig, ctx, hsh);
	CuAssert(tc, "Unable to verify signature.", res == KSI_OK);

	KSI_DataHash_free(hsh);
	KSI_Signature_free(sig);
	return;
}

static void Test_CreateSignatureWrongHMAC(CuTest* tc) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_DataHash *hsh = NULL;
	KSI_Signature *sig = NULL;

	KSI_ERR_clearErrors(ctx);


	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_CTX_setAggregator(ctx, conf.aggregator_url, "test-test", "tset-tset");
	CuAssert(tc, "Unable to spoil aggregator authentication data.", res == KSI_OK);

	/*Reset old aggregator password.*/
	res = KSI_Signature_create(ctx, hsh, &sig);
	KSI_CTX_setAggregator(ctx, conf.aggregator_url, conf.aggregator_user, conf.aggregator_pass);
	CuAssert(tc, "Unable to create signature.", res == KSI_SERVICE_AUTHENTICATION_FAILURE && sig == NULL);

	KSI_DataHash_free(hsh);
	KSI_Signature_free(sig);
	return;
}

static void Test_CreateSignatureUsingExtender(CuTest* tc) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_DataHash *hsh = NULL;
	KSI_Signature *sig = NULL;
	KSI_CTX *ctx = NULL;

	/* Create the context. */
	res = KSI_CTX_new(&ctx);
	CuAssert(tc, "Unable to create ctx.", res == KSI_OK && ctx != NULL);

	res = KSI_CTX_setAggregator(ctx, conf.extender_url, conf.extender_user, conf.extender_pass);
	CuAssert(tc, "Unable to set configure extender as aggregator.", res == KSI_OK);

	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_Signature_create(ctx, hsh, &sig);
	CuAssert(tc, "The creation of signature must fail.", sig == NULL);
	CuAssert(tc, "Invalid KSI status code for mixed up request.", res == KSI_HTTP_ERROR);
	CuAssert(tc, "External error (HTTP) must be 400.", ctx_get_base_external_error(ctx) == 400);


	KSI_DataHash_free(hsh);
	KSI_Signature_free(sig);
	KSI_CTX_free(ctx);
	return;
}

static void Test_CreateSignature_useProvider(CuTest* tc, const char *uri_host, unsigned port, const char *user, const char *key,
		int (*createProvider)(KSI_CTX *ctx, KSI_NetworkClient **http),
		int (*setAggregator)(KSI_NetworkClient *client, const char *url_host, unsigned port, const char *user, const char *pass)) {
	int res = KSI_UNKNOWN_ERROR;
	KSI_DataHash *hsh = NULL;
	KSI_Signature *sig = NULL;
	KSI_NetworkClient *client = NULL;
	KSI_CTX *ctx = NULL;

	/* Create the context. */
	res = KSI_CTX_new(&ctx);
	CuAssert(tc, "Unable to create ctx.", res == KSI_OK && ctx != NULL);

	res = createProvider(ctx, &client);
	CuAssert(tc, "Unable to create network client.", res == KSI_OK && client != NULL);

	res = setAggregator(client, uri_host, port, user, key);
	CuAssert(tc, "Unable to set aggregator.", res == KSI_OK);

	res = KSI_CTX_setNetworkProvider(ctx, client);
	CuAssert(tc, "Unable to set new network client.", res == KSI_OK);
	client = NULL;

	res = KSI_DataHash_fromDigest(ctx, KSI_getHashAlgorithmByName("sha256"), "c8ef6d57ac28d1b4e95a513959f5fcdd0688380a43d601a5ace1d2e96884690a", 32, &hsh);
	CuAssert(tc, "Unable to create hash.", res == KSI_OK && hsh != NULL);

	res = KSI_Signature_create(ctx, hsh, &sig);
	CuAssert(tc, "The creation of signature must not fail.", sig != NULL);


	KSI_NetworkClient_free(client);
	KSI_DataHash_free(hsh);
	KSI_Signature_free(sig);
	KSI_CTX_free(ctx);
	return;
}

static int http_setAggrWrapper(KSI_NetworkClient *client, const char *url_host, unsigned port, const char *user, const char *pass) {
	return KSI_HttpClient_setAggregator(client, url_host, user, pass);
}

static int uri_setAggrWrapper(KSI_NetworkClient *client, const char *url_host, unsigned port, const char *user, const char *pass) {
	return KSI_UriClient_setAggregator(client, url_host, user, pass);
}

static int tcp_setAggrWrapper(KSI_NetworkClient *client, const char *url_host, unsigned port, const char *user, const char *pass) {
	return KSI_TcpClient_setAggregator(client, url_host, port, user, pass);
}

static void Test_CreateSignatureDifferentProviders(CuTest* tc) {
	/* Http provider. */
	Test_CreateSignature_useProvider(tc,
			conf.aggregator_url, 0, conf.aggregator_user, conf.aggregator_pass,
			KSI_HttpClient_new,
			http_setAggrWrapper);

	/* Tcp provider. */
	Test_CreateSignature_useProvider(tc,
			conf.tcp_host, conf.tcp_port, conf.tcp_user, conf.tcp_pass,
			KSI_TcpClient_new,
			tcp_setAggrWrapper);

	/* Uri provider - all info is extracted from uri. */
	Test_CreateSignature_useProvider(tc,
			conf.aggregator_url, 0, NULL, NULL,
			KSI_UriClient_new,
			uri_setAggrWrapper);

	Test_CreateSignature_useProvider(tc,
			conf.tcp_url, 0, NULL, NULL,
			KSI_UriClient_new,
			uri_setAggrWrapper);
	return;
}

CuSuite* AggreIntegrationTests_getSuite(void) {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, Test_CreateSignatureDefProvider);
	SUITE_ADD_TEST(suite, Test_CreateSignatureWrongHMAC);
	SUITE_ADD_TEST(suite, Test_NOKAggr_TreeTooLarge);
	SUITE_ADD_TEST(suite, Test_TCPCreateSignatureDefProvider);
	SUITE_ADD_TEST(suite, Test_CreateSignatureUsingExtender);
	SUITE_ADD_TEST(suite, Test_CreateSignatureDifferentProviders);

	return suite;
}

