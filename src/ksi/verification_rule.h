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

#ifndef VERIFICATION_RULE_H_
#define VERIFICATION_RULE_H_

#include "ksi.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * This rule verifies that if RFC3161 record is present then the calculated output hash (from RFC3161 record) equals to
	 * aggregation chain input hash. If RFC3161 record is missing then the status {@link VerificationResultCode#OK} is
	 * returned.
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_AggregationChainInputHashVerification(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 * This rule verifies that all aggregation hash chains are consistent (e.g previous aggregation output hash equals to
	 * current aggregation chain input hash).
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_AggregationHashChainConsistency(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_AggregationHashChainTimeConsistency(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarHashChainInputHashVerification(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarHashChainAggregationTime(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarHashChainRegistrationTime(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarAuthenticationRecordAggregationHash(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarAuthenticationRecordAggregationTime(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_SignaturePublicationRecordPublicationHash(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_SignaturePublicationRecordPublicationTime(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_DocumentHashVerification(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_SignatureDoesNotContainPublication(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_ExtendedSignatureAggregationChainRightLinksMatches(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_SignaturePublicationRecordExistence(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_ExtendedSignatureCalendarChainRootHash(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarHashChainDoesNotExist(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_ExtendedSignatureCalendarChainInputHash(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_ExtendedSignatureCalendarChainAggregationTime(KSI_Signature *sig, KSI_RuleVerificationResult *result);

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */

	/**
	 *
	 * \param[in]	sig			KSI signature.
	 * \param[out]	result		Verification result.
	 *
	 * \return status code (#KSI_OK, when operation succeeded, otherwise an error code).
	 */
	int KSI_VerificationRule_CalendarHashChainExistence(KSI_Signature *sig, KSI_RuleVerificationResult *result);


#ifdef __cplusplus
}
#endif

#endif /* VERIFICATION_RULE_H_ */
