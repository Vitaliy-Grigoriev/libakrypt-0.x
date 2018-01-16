/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "KeyContainer"
 * 	found in "KeyContainer.asn1"
 * 	`asn1c -fwide-types -findirect-choice`
 */

#ifndef	_KeyProtectParameters_H_
#define	_KeyProtectParameters_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <OCTET_STRING.h>
#include "AlgorithmIdentifier.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* KeyProtectParameters */
typedef struct KeyProtectParameters {
	long	 iterationCount;
	OCTET_STRING_t	 encryptionSalt;
	AlgorithmIdentifier_t	 encryptionMode;
	OCTET_STRING_t	 encryptionIV;
	OCTET_STRING_t	 integritySalt;
	AlgorithmIdentifier_t	 integrityMode;
	OCTET_STRING_t	*integrityIV	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} KeyProtectParameters_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_KeyProtectParameters;

#ifdef __cplusplus
}
#endif

#endif	/* _KeyProtectParameters_H_ */
#include <asn_internal.h>
