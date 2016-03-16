/*
 * Copyright 2013-2016 Guardtime, Inc.
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

#ifndef POLICY_IMPL_H
#define	POLICY_IMPL_H

#include "internal.h"
#include "policy.h"
#include "list.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum RuleType_en {
	RULE_TYPE_BASIC,
	RULE_TYPE_COMPOSITE_AND,
	RULE_TYPE_COMPOSITE_OR
} RuleType;

typedef int (*Verifier)(KSI_VerificationContext *, KSI_RuleVerificationResult *);

typedef struct Rule_st {
	RuleType type;
	const void *rule;
} Rule;

KSI_DEFINE_LIST(Rule);

struct KSI_Policy_st {
	const Rule *rules;
	const KSI_Policy *fallbackPolicy;
};

typedef struct VerificationUserData_st {
	/** Signature to be verified */
	KSI_Signature *sig;

	/** Indicates whether signature extention is allowed */
	int extendingAllowed;

	/** Initial aggregation level. */
	KSI_uint64_t docAggrLevel;

	/** Document hash to be verified. */
	KSI_DataHash *documentHash;

	/** Publication string to be used. */
	KSI_PublicationData *userPublication;

	/** Publication file to be used. */
	KSI_PublicationsFile *userPublicationsFile;
} VerificationUserData;

typedef struct VerificationTempData_st {

	/** Temporary extended signature */
	KSI_Signature *extendedSig;

	/** Publicationsfile to be used. The memory may not be freed! */
	KSI_PublicationsFile *publicationsFile;

	/** Signature aggregation output hash (calendar chain input hash) */
	KSI_DataHash *aggregationOutputHash;
} VerificationTempData;

struct KSI_VerificationContext_st {
	KSI_CTX *ctx;

	VerificationUserData userData;

	VerificationTempData tempData;
};

#ifdef	__cplusplus
}
#endif

#endif	/* POLICY_IMPL_H */
