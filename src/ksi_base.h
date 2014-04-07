#ifndef _KSI_BASE_H_
#define _KSI_BASE_H_

#include <stdio.h>

/* Returns true if context has no errors. */
#define KSI_CTX_OK(ctx) ((ctx) != NULL && (ctx)->statusCode == KSI_OK && (ctx)->errors_count == 0)

#ifdef __cplusplus
extern "C" {
#endif


typedef struct KSI_CTX_st KSI_CTX;
typedef struct KSI_ERR_st KSI_ERR;
typedef struct KSI_TLV_st KSI_TLV;
typedef struct KSI_Signature_st KSI_Signature;
typedef struct KSI_NetProvider_st KSI_NetProvider;

/**************
 * KSI INTEGER
 **************/
typedef struct KSI_Integer_st KSI_Integer;
#define KSI_uint64_t uint64_t

void KSI_Integer_free(KSI_Integer *kint);
int KSI_Integer_getSize(KSI_Integer *kint, int *size);
int KSI_Integer_getUInt64(KSI_Integer *kint, KSI_uint64_t *val);
int KSI_Integer_new(KSI_CTX *ctx, KSI_uint64_t value, KSI_Integer **kint);
int KSI_Integer_equals(KSI_Integer *a, KSI_Integer *b);
int KSI_Integer_equalsUInt(KSI_Integer *o, KSI_uint64_t i);

/*************
 * KSI READER
 *************/
/* KSI reader type. */
typedef struct KSI_RDR_st KSI_RDR;


enum KSI_StatusCode {
/* RETURN CODES WHICH ARE NOT ERRORS */
	KSI_OK = 0,

/* SYNTAX ERRORS */
	/**
	 * Argument to function was invalid. Mostly this indicates \c NULL
	 * pointer.
	 */
	KSI_INVALID_ARGUMENT = 0x00000100,
	/**
	 * Either arguments to function or responses from timestamping server had
	 * invalid format.
	 */
	KSI_INVALID_FORMAT,
	/**
	 * The given hash algorithm is considered untrustworthy by
	 * the verification policy.
	 */
	KSI_UNTRUSTED_HASH_ALGORITHM,
	KSI_UNAVAILABLE_HASH_ALGORITHM,
	/**
	 * Buffer too small to perform operation.
	 */
	KSI_BUFFER_OVERFLOW,
	/**
	 * TLV payload has wrong type for operation.
	 */
	KSI_TLV_PAYLOAD_TYPE_MISMATCH,

	/**
	 * The async operation has not finished.
	 */
	KSI_ASYNC_NOT_FINISHED,

/* SYSTEM ERRORS */
	KSI_OUT_OF_MEMORY = 0x00000300,
	KSI_IO_ERROR,
	KSI_NETWORK_ERROR,
	KSI_HTTP_ERROR,
	/**
	 * Cryptographic operation could not be performed. Likely causes are
	 * unsupported cryptographic algorithms, invalid keys and lack of
	 * resources.
	 */
	KSI_CRYPTO_FAILURE,


	KSI_UNKNOWN_ERROR
};

const char *KSI_getErrorString(int statusCode);

/**
 * Initialize KSI context #KSI_CTX
 */

int KSI_CTX_new(KSI_CTX **context);

/**
 * Free KSI context.
 */
void KSI_CTX_free(KSI_CTX *context);

/****************************
 *  ERROR HANDLING FUNCTIONS.
 ****************************/

/**
 * Get the last status set.
 */
int KSI_ERR_getStatus(KSI_CTX *ctx);

/**
 * Dump error stack trace to stream
 */
int KSI_ERR_statusDump(KSI_CTX *ctx, FILE *f);

/****************
 * LOG FUNCTIONS.
 ****************/

// TODO!

/**
 * Set log file.
 *
 * \note this method will append to the file if it exists.
 */
int KSI_LOG_init(KSI_CTX *ctx, char *fileName, int logLevel);

/**
 * Change the log level.
 */
int KSI_LOG_setLevel(int logLevel);

/***********
 *
 * DATA HASH
 *
 ***********/

/**
 * This structure is used for calculating the hash values.
 * \see #KSI_DataHash, #KSI_DataHasher_open, #KSI_DataHasher_reset, #KSI_DataHasher_close, #KSI_DataHasher_free
 */
typedef struct KSI_DataHasher_st KSI_DataHasher;

typedef struct KSI_HashChain_MetaHash_st KSI_MetaHash;

typedef struct KSI_HashChain_MetaHash_st KSI_MetaData;


/**
 * This structure represents hashed data.
 * \see #KSI_DataHasher, #KSI_DataHasher_close, #KSI_DataHash_free
 */
typedef struct KSI_DataHash_st KSI_DataHash;

int KSI_DataHash_equals(KSI_DataHash *left, KSI_DataHash *right);

/**
 * The Guardtime representation of hash algorithms, necessary to calculate
 * instances of #KSI_DataHasher and #KSI_DataHash.
 */
enum KSI_HashAlgorithm {
	/** The SHA-1 algorithm. */
	KSI_HASHALG_SHA1 = 0,
	/** The SHA-256 algorithm. */
	KSI_HASHALG_SHA2_256,
	/** The RIPEMD-160 algorithm. */
	KSI_HASHALG_RIPEMD160,
	/** The SHA-224 algorithm. */
	KSI_HASHALG_SHA2_224,
	/** The SHA-384 algorithm. */
	KSI_HASHALG_SHA2_384,
	/** The SHA-512 algorithm. */
	KSI_HASHALG_SHA2_512,
	/** The RIPEMD-256 algorithm. */
	KSI_HASHALG_RIPEMD_256,
	/** The SHA3-244 algorithm. */
	KSI_HASHALG_SHA3_244,
	/** The SHA3-256 algorithm. */
	KSI_HASHALG_SHA3_256,
	/** The SHA3-384 algorithm. */
	KSI_HASHALG_SHA3_384,
	/** The SHA3-512 algoritm */
	KSI_HASHALG_SHA3_512,
	/** The SM3 algorithm.*/
	KSI_HASHALG_SM3,

	/* Number of known hash algorithms. */
	KSI_NUMBER_OF_KNOWN_HASHALGS,

	/** Use default algorithm. */
	KSI_HASHALG_DEFAULT = -1
};

int KSI_global_init(void);

void KSI_global_finalize(void);

/**
 * Starts a hash computation.
 * \see #KSI_DataHasher_add, #KSI_DataHasher_close
 *
 * \param[in] ctx Ksi context.
 * \param[in] hash_algorithm Identifier of the hash algorithm.
 * See #KSI_HashAlgorithm for possible values.
 * \param[out] hasher Pointer that will receive pointer to the
 * hasher object.
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHasher_open(KSI_CTX *ctx, int hash_algorithm, KSI_DataHasher **hasher);

/**
 * Resets the state of the hash computation.
 * \see #KSI_DataHasher_open, #KSI_DataHasher_close
 *
 * \param[in] hasher Pointer to the hasher.
 */
int KSI_DataHasher_reset(KSI_DataHasher *hasher);

/**
 * Adds data to an open hash computation.
 * \see #KSI_DataHasher_open, #KSI_GTDataHasher_close
 *
 * \param[in] hasher Pointer to the hasher object.
 * \param data \c (in) - Pointer to the data to be hashed.
 * \param data_length \c (in) - Length of the hashed data.
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHasher_add(KSI_DataHasher *hasher, const void *data, size_t data_length);

/**
 * Finalizes a hash computation.
 * \see #KSI_DataHasher_open, #KSI_DataHasher_add, #KSI_DataHasher_free
 *
 * \param[in] hasher	Pointer to the hasher object.
 * \param[out] hash		Pointer that will receive pointer to the hash object.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHasher_close(KSI_DataHasher *hasher, KSI_DataHash **hash);

/**
 * Frees memory used by hasher.
 *
 * \param data_hash \c (in) - \c GTDataHash object that is to be freed.
 *
 * \see #KSI_free()
 */
void KSI_DataHash_free(KSI_DataHash *hash);

/**
 * Calculates the data hash object from the input data.
 *
 * @param[in]		ctx				KSI context.
 * @param[in]		data			Pointer to the input data.
 * @param[in]		data_length		Length of the imput data.
 * @param[in]		hash_id			Hash algorithm id.
 * @param[out]		hash			Pointer to the pointer receiving the data hash object.
 *
 * @return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_create(KSI_CTX *ctx, const void *data, size_t data_length, int hash_id, KSI_DataHash **hash);

/**
 * Creates a clone of the data hash.
 *
 * @param[in]	from	Data hash to be cloned.
 * @param[out]	to		Pointer to the receiving pointer to the cloned object.
 *
 * @return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_clone(KSI_DataHash *from, KSI_DataHash **to);

/**
 * Interneal data access method.
 *
 * \param[in]	hash			Data hash object.
 * \param[out]	algorithm		Algorithm used to compute the hash.
 * \param[out]	digest			Binary digest value.
 * \param[out]	digest_length	Length of the digest value.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 *
 * \note The digest value returned by this function has to be freed by the
 * programmer with #KSI_free.
 */
int KSI_DataHash_getData(KSI_DataHash *hash, int *hash_id, const unsigned char **digest, int *digest_length);

/**
 * Constructor for #KSI_DataHash object from existing hash value.
 * \param[in]	ctx				KSI context.
 * \param[in]	algorithm		Algorithm used to compute the digest value.
 * \param[in]	digest			Binary digest value.
 * \param[in]	digest_length	Lengt of the binary digest value.
 * \param[in]	hash			Pointer that will receive pointer to the hash object.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_fromDigest(KSI_CTX *ctx, int hash_id, const unsigned char *digest, int digest_length, KSI_DataHash **hash);

/**
 * Reevaluates the #KSI_DataHash object with another precalculated hash value.
 * \param[in]	algorithm		Algorithm used to compute the digest value.
 * \param[in]	digest			Binary digest value.
 * \param[in]	digest_length	Lengt of the binary digest value.
 * \param[in]	hash			Pointer to the existing hash object.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_fromData_ex(int hash_id, const unsigned char *digest, int digest_length, KSI_DataHash *hash);

/**
 * Encodes the data hash object as an imprtint.
 *
 * \param[in]	hash			Data hash object.
 * \param[out]	imprint			Pointer that will receive pointer to the imprint.
 * \param[out]	imprint_length	Pointer that will reveive the length of the imprint.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_getImprint(KSI_DataHash *hash, const unsigned char **imprint, int *imprint_length);

/**
 * Encodes the data hash object as an imprint into an existing array.
 *
 * @param[in]	hash			Data hash object.
 * @param[in]	target			Pointer to the existing target memory.
 * @param[in]	target_size		Maximum output length.
 * @param[out]	target_length	Length of the serialized imprint.
 */
int KSI_DataHash_getImprint_ex(KSI_DataHash *hash, unsigned char *target, int target_size, int *target_length);

/**
 * Constructor for #KSI_DataHash object from existing imprint.
 *
 * \param[in]	ctx				KSI context.
 * \param[in]	imprint			Pointer to the imprint.
 * \param[in]	imprint_length	Length of the imprint.
 * \param[out]	hash			Pointer that will receive pointer to the data hash objet.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_fromImprint(KSI_CTX *ctx, unsigned char *imprint, int imprint_length, KSI_DataHash **hash);

/**
 * Reevaluates the existing #KSI_DataHash object.
 *
 * \param[in]	imprint			Pointer to hash imprint.
 * \param[in]	imprint_length	Length of the imprint.
 * \param[out]	hash			Pointer to the data hash object.
 *
 * \return status code (\c KSI_OK, when operation succeeded, otherwise an
 * error code).
 */
int KSI_DataHash_fromImprint_ex(unsigned char *imprint, int imprint_length, KSI_DataHash *hash);

/**
 *
 */
const char *KSI_getHashAlgorithmName(int hash_algorithm);

/**
 *
 */
int KSI_getHashAlgorithmByName(const char *name);

/**
 *
 */
int KSI_DataHash_create(KSI_CTX *ctx, const void *data, size_t data_length, int hash_id, KSI_DataHash **hash);

/**
 *
 */
int KSI_DataHash_clone(KSI_DataHash *from, KSI_DataHash **to);
/**
 *
 */
int KSI_DataHash_getCtx(KSI_DataHash *hsh, KSI_CTX **ctx);

/**
 * Returns the hash length in bytes for the given hash algorithm id.
 *
 * \param[in]	hash_id		Hash algorithm id
 *
 * \return Length of the hash value calculated by the given hash algorithm. Returns negative value on error.
 */
int KSI_getHashLength(int hash_id);

/**
 * Fixes hash algorithm ID: replaces default ID with the current default
 * as necessary.
 **/
int KSI_fixHashAlgorithm(int hash_id);

/**
 * Is \p hash_id hash algorithm trusted?
 * @param[in]	hash_id		Hash algorithm id.
 *
 * @return Returns 0 if algorithm is not trusted, otherwise non-zero.
 */
int KSI_isTrusteddHashAlgorithm(int hash_id);

/**
 * Is \p hash_id hash algorithm supported by the API.
 * @param[in]	hash_id		Hash algorithm id.
 *
 * @return Returns 0 if algorithm is not supported, otherwise non-zero.
 */
int KSI_isSupportedHashAlgorithm(int hash_id);

/**
 * Returns a pointer to constant string containing the name of the hash algorithm. Returns NULL if
 * the algorithm is unknown.
 *
 * \param[in]	hash_algorithm	The hash algorithm id.
 *
 * \return Name of the algorithm or NULL on error.
 */
const char *KSI_getHashAlgorithmName(int hash_algorithm);

/**
 * Returns the hash algorithm id for the given name.
 *
 * \param[in]	name	Hash algorithm name.
 *
 * \return Hash algorithm id or -1 on error.
 */
int KSI_getHashAlgorithmByName(const char *name);

/************
 *
 * NETWORKING
 *
 ************/

/** Transport Providers */
int KSI_NET_CURL_new(KSI_CTX *ctx, KSI_NetProvider **netProvider);
int KSI_NET_CURL_setSignerUrl(KSI_NetProvider *netProvider, char *val);
int KSI_NET_CURL_setExtenderUrl(KSI_NetProvider *netProvider, char *val);
int KSI_NET_CURL_setPublicationUrl(KSI_NetProvider *netProvider, char *val);
int KSI_NET_CURL_setConnectionTimeoutSeconds(KSI_NetProvider *netProvider, int val);
int KSI_NET_CURL_setReadTimeoutSeconds(KSI_NetProvider *netProvider, int val);

/**
 * Network resource handle
 *
 *	\see #KSI_NET_sendRequest
 */
typedef struct KSI_NetHandle_st KSI_NetHandle;

/**
 *
 */
int KSI_NET_sendRequest(KSI_CTX *ctx, const char *url, const unsigned char *request, int request_length, KSI_NetHandle **handle);

/**
 *
 */
int KSI_NET_sendSignRequest(KSI_NetProvider *netProvider, const unsigned char *request, int request_length, KSI_NetHandle **handle);
/**
 *
 */
int KSI_NET_sendExtendRequest(KSI_CTX *ctx, const unsigned char *request, int request_length, KSI_NetHandle **handle);

/**
 *
 */
int KSI_NET_sendPublicationRequest(KSI_CTX *ctx, const unsigned char *request, int request_length, KSI_NetHandle **handle);

/**
 *
 */
int KSI_NET_getResponse(KSI_NetHandle *handle, unsigned char **response, int *response_length, int copy);

/**
 *
 */
void KSI_NetHandle_free(KSI_NetHandle *heandle);

/**
 *
 */
int KSI_CTX_setNetworkProvider(KSI_CTX *ctx, KSI_NetProvider *netProvider);
void KSI_NetProvider_free(KSI_NetProvider *provider);

void KSI_Signature_free(KSI_Signature *sig);
int KSI_Signature_getDataHash(KSI_Signature *sig, const KSI_DataHash ** hsh);
int KSI_Signature_getSigningTime(KSI_Signature *sig, KSI_Integer *signTime);
int KSI_Signature_getSignerIdentity(KSI_Signature *sig, char **identity);
int KSI_Signature_getCalendarHash(KSI_Signature *sig, const KSI_DataHash **hsh);
/** TODO! For now these are just mock declarations
int KSI_Signature_getPublishedData(KSI_Signature *sig, char **pub_data);
int KSI_Signature_getPublicationReference(KSI_Signature *sig, char **pub_ref);
int KSI_Signature_getPublicationSignature(KSI_Signature *sig, char **pub_sig);
*/

void *KSI_malloc(size_t size);
void *KSI_calloc(size_t num, size_t size);
void *KSI_realloc(void *ptr, size_t size);
void KSI_free(void *ptr);

/**********
 * UTIL's
 **********/

int KSI_decodeHexStr(const char *hexstr, unsigned char *buf, int buf_size, int *buf_length);

#ifdef __cplusplus
}
#endif

#endif
