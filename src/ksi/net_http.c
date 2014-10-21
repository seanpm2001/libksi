#include <string.h>
#include "net_http_impl.h"
#include <assert.h>

static int setStringParam(char **param, const char *val) {
	char *tmp = NULL;
	int res = KSI_UNKNOWN_ERROR;


	tmp = KSI_calloc(strlen(val) + 1, 1);
	if (tmp == NULL) {
		res = KSI_INVALID_ARGUMENT;
		goto cleanup;
	}
	memcpy(tmp, val, strlen(val) + 1);

	if (*param != NULL) {
		KSI_free(*param);
	}

	*param = tmp;
	tmp = NULL;

	res = KSI_OK;

cleanup:

	KSI_free(tmp);

	return res;
}

static int setIntParam(int *param, int val) {
	*param = val;
	return KSI_OK;
}

static void debug_saw_raw_data_to_file(char *fname, unsigned char *data, unsigned data_len){
	FILE *out = NULL;
	out = fopen(fname, "wb");
	fwrite(data, 1, data_len, out);
	fclose(out);
}

static int postProcessRequest(KSI_HttpClientCtx *http, void *req, void* pdu, const char *user, int (*getHeader)(const void *, KSI_Header **), int (*setHeader)(void *, KSI_Header *), int (*getId)(void *, KSI_Integer **), int (*setId)(void *, KSI_Integer *)) {
	KSI_ERR err;
	int res;
	KSI_uint64_t reqId = 0;
	KSI_Header *header = NULL;
	KSI_Integer *instanceId = NULL;
	KSI_Integer *messageId = NULL;
	KSI_Integer *requestId = NULL;
	KSI_OctetString *client_id = NULL;
	
	KSI_PRE(&err, http != NULL) goto cleanup;
	KSI_PRE(&err, req != NULL) goto cleanup;
	KSI_PRE(&err, pdu != NULL) goto cleanup;
	KSI_PRE(&err, user != NULL) goto cleanup;
	KSI_BEGIN(http->ctx, &err);
	
	
	reqId = ++http->requestId;
	
	res = getHeader(pdu, &header);
	KSI_CATCH(&err, res) goto cleanup;

	/*Add header*/
	if (header == NULL) {
		res = KSI_Header_new(http->ctx, &header);
		KSI_CATCH(&err, res) goto cleanup;

		res = KSI_Integer_new(http->ctx, (KSI_uint64_t)http, &instanceId);
		KSI_CATCH(&err, res) goto cleanup;

		res = KSI_Integer_new(http->ctx, reqId, &messageId);
		KSI_CATCH(&err, res) goto cleanup;

		res = KSI_OctetString_new(http->ctx, user, strlen(user), &client_id);
		KSI_CATCH(&err, res) goto cleanup;
		
		res = KSI_Header_setInstanceId(header, instanceId);
		KSI_CATCH(&err, res) goto cleanup;
		instanceId = NULL;

		res = KSI_Header_setMessageId(header, messageId);
		KSI_CATCH(&err, res) goto cleanup;
		messageId = NULL;

		res = KSI_Header_setClientId(header, client_id);
		KSI_CATCH(&err, res) goto cleanup;
		client_id = NULL;
		
		res = setHeader(pdu, header);
		KSI_CATCH(&err, res) goto cleanup;
		header = NULL;
	}
	
	res = getId(req, &requestId);
	KSI_CATCH(&err, res) goto cleanup;
	if (requestId == NULL) {
		res = KSI_Integer_new(http->ctx, reqId, &requestId);
		KSI_CATCH(&err, res) goto cleanup;

		res = setId(req, requestId);
		KSI_CATCH(&err, res) goto cleanup;
		requestId = NULL;
	}
	
	KSI_SUCCESS(&err);

cleanup:

	KSI_Integer_free(messageId);
	KSI_Integer_free(instanceId);
	KSI_Integer_free(requestId);
	KSI_Header_free(header);
	KSI_OctetString_free(client_id);
	
	return KSI_RETURN(&err);
}


static int prepareAggregationRequest(KSI_NetworkClient *client, KSI_AggregationReq *req, KSI_RequestHandle **handle) {
	KSI_ERR err;
	int res;
	KSI_HttpClientCtx *http = NULL;
	KSI_RequestHandle *tmp = NULL;
	KSI_AggregationPdu *pdu = NULL;
	unsigned char *raw = NULL;
	unsigned raw_len = 0;

	KSI_PRE(&err, client != NULL) goto cleanup;
	KSI_PRE(&err, req != NULL) goto cleanup;
	KSI_PRE(&err, handle != NULL) goto cleanup;
	KSI_BEGIN(client->ctx, &err);

	http = client->implCtx;

	if (http == NULL) {
		KSI_FAIL(&err, KSI_INVALID_ARGUMENT, "KSI_HttpClient context not initialized.");
		goto cleanup;
	}
	res = KSI_AggregationPdu_new(client->ctx, &pdu);
	KSI_CATCH(&err, res) goto cleanup;
	
	res = postProcessRequest(http, req, pdu, http->agrUser, (int (*)(const void *, KSI_Header **))KSI_AggregationPdu_getHeader, (int (*)(void *, KSI_Header *))KSI_AggregationPdu_setHeader, (int (*)(void*, KSI_Integer**))KSI_AggregationReq_getRequestId, (int (*)(void*, KSI_Integer**))KSI_AggregationReq_setRequestId);
	KSI_CATCH(&err, res) goto cleanup;

	res = KSI_AggregationPdu_setRequest(pdu, req);
	KSI_CATCH(&err, res) goto cleanup;

	res = KSI_AggregationPdu_calculateHmac(pdu, KSI_HASHALG_SHA1, http->agrPass);
	KSI_CATCH(&err, res) goto cleanup;
	
	res = KSI_AggregationPdu_serialize(pdu, &raw, &raw_len);
	KSI_CATCH(&err, res) goto cleanup;

	KSI_LOG_logBlob(client->ctx, KSI_LOG_DEBUG, "Aggregation request", raw, raw_len);

	/* Create a new request handle */
	res = KSI_RequestHandle_new(client->ctx, raw, raw_len, &tmp);
	KSI_CATCH(&err, res) goto cleanup;

	/* Evaluate the handle object. */
	if (http->sendRequest == NULL) {
		KSI_FAIL(&err, KSI_UNKNOWN_ERROR, "Send request not initialized.");
		goto cleanup;
	}
	res = http->sendRequest(client, tmp, http->urlSigner);
	KSI_CATCH(&err, res) goto cleanup;

	*handle = tmp;
	tmp = NULL;

	KSI_SUCCESS(&err);

cleanup:

	if(pdu){
		/* Detach request from the PDU, as it may not be freed in this function. */
		KSI_AggregationPdu_setRequest(pdu, NULL);
		KSI_AggregationPdu_free(pdu);
	}

	KSI_RequestHandle_free(tmp);
	KSI_free(raw);

	return KSI_RETURN(&err);
}

static int prepareExtendRequest(KSI_NetworkClient *client, KSI_ExtendReq *req, KSI_RequestHandle **handle) {
	KSI_ERR err;
	int res;
	KSI_HttpClientCtx *http = NULL;
	KSI_RequestHandle *tmp = NULL;
	KSI_ExtendPdu *pdu = NULL;
	unsigned char *raw = NULL;
	unsigned raw_len = 0;


	KSI_PRE(&err, client != NULL) goto cleanup;
	KSI_PRE(&err, req != NULL) goto cleanup;
	KSI_PRE(&err, handle != NULL) goto cleanup;
	KSI_BEGIN(client->ctx, &err);

	http = client->implCtx;

	if (http == NULL) {
		KSI_FAIL(&err, KSI_INVALID_ARGUMENT, "KSI_HttpClient context not initialized.");
		goto cleanup;
	}
	res = KSI_ExtendPdu_new(client->ctx, &pdu);
	KSI_CATCH(&err, res) goto cleanup;
	
	res = postProcessRequest(http, req, pdu, http->extUser, (int (*)(const void *, KSI_Header **))KSI_ExtendPdu_getHeader, (int (*)(void *, KSI_Header *))KSI_ExtendPdu_setHeader, (int (*)(void*, KSI_Integer**))KSI_ExtendReq_getRequestId, (int (*)(void*, KSI_Integer*))KSI_ExtendReq_setRequestId);
	KSI_CATCH(&err, res) goto cleanup;
	
	res = KSI_ExtendPdu_setRequest(pdu, req);
	KSI_CATCH(&err, res) goto cleanup;

	res = KSI_ExtendPdu_calculateHmac(pdu, KSI_HASHALG_SHA1, http->extPass);
	KSI_CATCH(&err, res) goto cleanup;
	
	res = KSI_ExtendPdu_serialize(pdu, &raw, &raw_len);
	KSI_CATCH(&err, res) goto cleanup;

	KSI_LOG_logBlob(client->ctx, KSI_LOG_DEBUG, "Extending request", raw, raw_len);

	/* Create a new request handle */
	res = KSI_RequestHandle_new(client->ctx, raw, raw_len, &tmp);
	KSI_CATCH(&err, res) goto cleanup;

	if (http->sendRequest == NULL) {
		KSI_FAIL(&err, KSI_UNKNOWN_ERROR, "Send request not initialized.");
		goto cleanup;
	}

	res = http->sendRequest(client, tmp, http->urlExtender);
	KSI_CATCH(&err, res) goto cleanup;

	*handle = tmp;
	tmp = NULL;

	KSI_SUCCESS(&err);

cleanup:

	if(pdu){
		/* Detach request from the PDU, as it may not be freed in this function. */
		KSI_ExtendPdu_setRequest(pdu, NULL);
		KSI_ExtendPdu_free(pdu);
	}

	KSI_RequestHandle_free(tmp);
	KSI_free(raw);

	return KSI_RETURN(&err);
}

static int preparePublicationsFileRequest(KSI_NetworkClient *client, KSI_RequestHandle *handle) {
	KSI_ERR err;
	int res;
	KSI_HttpClientCtx *http = NULL;

	KSI_PRE(&err, client != NULL) goto cleanup;
	KSI_PRE(&err, handle != NULL) goto cleanup;
	KSI_BEGIN(client->ctx, &err);

	http = client->implCtx;

	if (http->sendRequest == NULL) {
		KSI_FAIL(&err, KSI_UNKNOWN_ERROR, "Send request not initialized.");
		goto cleanup;
	}
	res = http->sendRequest(client, handle, http->urlPublication);
	if (res != KSI_OK) goto cleanup;

	res = KSI_OK;

cleanup:

	return res;
}


static void httpClientCtx_free(KSI_HttpClientCtx *http) {
	if (http != NULL) {
		KSI_free(http->urlSigner);
		KSI_free(http->urlExtender);
		KSI_free(http->urlPublication);
		KSI_free(http->agentName);
		KSI_free(http->extUser);
		KSI_free(http->extPass);
		KSI_free(http->agrUser);
		KSI_free(http->agrPass);
		
		if (http->implCtx_free != NULL) http->implCtx_free(http->implCtx);
		KSI_free(http);
	}
}

static int httpClientCtx_new(KSI_CTX *ctx, KSI_HttpClientCtx **http) {
	KSI_HttpClientCtx *tmp = NULL;
	int res;

	tmp = KSI_new (KSI_HttpClientCtx);
	if (tmp == NULL) {
		res = KSI_OUT_OF_MEMORY;
		goto cleanup;
	}

	tmp->ctx = ctx;
	tmp->agentName = NULL;
	tmp->sendRequest = NULL;
	tmp->urlExtender = NULL;
	tmp->urlPublication = NULL;
	tmp->urlSigner = NULL;
	tmp->extUser = NULL;
	tmp->extPass = NULL;
	tmp->agrUser = NULL;
	tmp->agrPass = NULL;
	
	
	setIntParam(&tmp->connectionTimeoutSeconds, 10);
	setIntParam(&tmp->readTimeoutSeconds, 10);
	setStringParam(&tmp->urlSigner, KSI_DEFAULT_URI_AGGREGATOR);
	setStringParam(&tmp->urlExtender, KSI_DEFAULT_URI_EXTENDER);
	setStringParam(&tmp->urlPublication, KSI_DEFAULT_URI_PUBLICATIONS_FILE);
	setStringParam(&tmp->agentName, "KSI HTTP Client");
	setStringParam(&tmp->extUser, "anon");
	setStringParam(&tmp->extPass, "anon");
	setStringParam(&tmp->agrUser, "anon");
	setStringParam(&tmp->agrPass, "anon");
	
	tmp->requestId = 0;
	tmp->implCtx = NULL;
	tmp->implCtx_free = NULL;

	*http = tmp;
	tmp = NULL;

	res = KSI_OK;

cleanup:

	httpClientCtx_free(tmp);

	return res;
}

/**
 *
 */
int KSI_HttpClient_new(KSI_CTX *ctx, KSI_NetworkClient **netProvider) {
	KSI_ERR err;
	KSI_NetworkClient *pr = NULL;
	KSI_HttpClientCtx *http = NULL;
	int res;

	KSI_PRE(&err, ctx != NULL) goto cleanup;
	KSI_BEGIN(ctx, &err);

	res = KSI_NetworkClient_new(ctx, &pr);
	KSI_CATCH(&err, res) goto cleanup;

	res = httpClientCtx_new(ctx, &http);
	KSI_CATCH(&err, res) goto cleanup;

	pr->sendSignRequest = prepareAggregationRequest;
	pr->sendExtendRequest = prepareExtendRequest;
	pr->sendPublicationRequest = preparePublicationsFileRequest;
	pr->implCtx = http;
	pr->implCtx_free = (void (*)(void*))httpClientCtx_free;
	http = NULL;

	res = KSI_HttpClient_init(pr);
	KSI_CATCH(&err, res) goto cleanup;

	*netProvider = pr;
	pr = NULL;

	KSI_SUCCESS(&err);

cleanup:

	KSI_NetworkClient_free(pr);
	httpClientCtx_free(http);

	return KSI_RETURN(&err);
}


#define KSI_NET_IMPLEMENT_SETTER(name, type, var, fn) 														\
		int KSI_HttpClient_set##name(KSI_NetworkClient *client, type val) {								\
			int res = KSI_UNKNOWN_ERROR;																\
			KSI_HttpClientCtx *pctx = NULL;																\
			if (client == NULL) {																		\
				res = KSI_INVALID_ARGUMENT;																\
				goto cleanup;																			\
			}																							\
			res = KSI_NetworkClient_getNetContext(client, (void **)&pctx);								\
			if (res != KSI_OK) goto cleanup;															\
			res = (fn)(&pctx->var, val);																\
		cleanup:																						\
			return res;																					\
		}																								\

KSI_NET_IMPLEMENT_SETTER(SignerUrl, const char *, urlSigner, setStringParam);
KSI_NET_IMPLEMENT_SETTER(ExtenderUrl, const char *, urlExtender, setStringParam);
KSI_NET_IMPLEMENT_SETTER(PublicationUrl, const char *, urlPublication, setStringParam);
KSI_NET_IMPLEMENT_SETTER(ConnectTimeoutSeconds, int, connectionTimeoutSeconds, setIntParam);
KSI_NET_IMPLEMENT_SETTER(ReadTimeoutSeconds, int, readTimeoutSeconds, setIntParam);
KSI_NET_IMPLEMENT_SETTER(ExtenderUser, const char *, extUser, setStringParam);
KSI_NET_IMPLEMENT_SETTER(ExtenderPass, const char *, extPass, setStringParam);
KSI_NET_IMPLEMENT_SETTER(AggregatoUser, const char *, agrUser, setStringParam);
KSI_NET_IMPLEMENT_SETTER(AggregatoPass, const char *, agrPass, setStringParam);
