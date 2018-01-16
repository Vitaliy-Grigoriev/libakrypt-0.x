/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "KeyContainer"
 * 	found in "KeyContainer.asn1"
 * 	`asn1c -fwide-types -findirect-choice`
 */

#include "KeyContainer.h"

static int
memb_version_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int asn_DFL_2_set_1(int set_value, void **sptr) {
	long *st = *sptr;
	
	if(!st) {
		if(!set_value) return -1;	/* Not a default value */
		st = (*sptr = CALLOC(1, sizeof(*st)));
		if(!st) return -1;
	}
	
	if(set_value) {
		/* Install default value 1 */
		*st = 1;
		return 0;
	} else {
		/* Test default value 1 */
		return (*st == 1);
	}
}
static asn_TYPE_member_t asn_MBR_keys_3[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_AbstractKey,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static const ber_tlv_tag_t asn_DEF_keys_tags_3[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_keys_specs_3 = {
	sizeof(struct keys),
	offsetof(struct keys, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_keys_3 = {
	"keys",
	"keys",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_keys_tags_3,
	sizeof(asn_DEF_keys_tags_3)
		/sizeof(asn_DEF_keys_tags_3[0]), /* 1 */
	asn_DEF_keys_tags_3,	/* Same as above */
	sizeof(asn_DEF_keys_tags_3)
		/sizeof(asn_DEF_keys_tags_3[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_keys_3,
	1,	/* Single element */
	&asn_SPC_keys_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_KeyContainer_1[] = {
	{ ATF_POINTER, 1, offsetof(struct KeyContainer, version),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		memb_version_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		asn_DFL_2_set_1,	/* DEFAULT 1 */
		"version"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct KeyContainer, keys),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_keys_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keys"
		},
};
static const ber_tlv_tag_t asn_DEF_KeyContainer_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_KeyContainer_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 0 }, /* version */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 } /* keys */
};
static asn_SEQUENCE_specifics_t asn_SPC_KeyContainer_specs_1 = {
	sizeof(struct KeyContainer),
	offsetof(struct KeyContainer, _asn_ctx),
	asn_MAP_KeyContainer_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_KeyContainer = {
	"KeyContainer",
	"KeyContainer",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_KeyContainer_tags_1,
	sizeof(asn_DEF_KeyContainer_tags_1)
		/sizeof(asn_DEF_KeyContainer_tags_1[0]), /* 1 */
	asn_DEF_KeyContainer_tags_1,	/* Same as above */
	sizeof(asn_DEF_KeyContainer_tags_1)
		/sizeof(asn_DEF_KeyContainer_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_KeyContainer_1,
	2,	/* Elements count */
	&asn_SPC_KeyContainer_specs_1	/* Additional specs */
};

