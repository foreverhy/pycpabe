#ifndef ABE_PARSE_POLICY_LANG_H
#define ABE_PARSE_POLICY_LANG_H
/*
	Include common.h before including this file.
*/

#ifdef __cplusplus
extern "C" {
#endif

char* parse_policy_lang( char* s );
void  parse_attribute( GSList** l, char* a );

#ifdef __cplusplus
}
#endif

#endif
