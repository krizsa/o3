
#define LIBXML_SAX1_ENABLED 1
#define LIBXML_XPATH_ENABLED 1
#define LIBXML_OUTPUT_ENABLED 1


#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>


#include "iXml.h"
#include "cXmlNodeList.h"
#include "cXmlNamedNodeMap.h"
#include "cXmlNode.h"
#include "cXmlAttr.h"
#include "cXmlNodeArray.h"
#include "cXmlElement.h"
#include "cXmlCharacterData.h"
#include "cXmlText.h"
#include "cXmlComment.h"
#include "cXmlCDATASection.h"
#include "cXmlDocument.h"
#include "cXml.h"

#ifdef O3_WITH_GLUE
#include "cXmlNodeList_glue.h"
#include "cXmlNamedNodeMap_glue.h"
#include "cXmlNode_glue.h"
#include "cXmlAttr_glue.h"
#include "cXmlNodeArray_glue.h"
#include "cXmlElement_glue.h"
#include "cXmlCharacterData_glue.h"
#include "cXmlText_glue.h"
#include "cXmlComment_glue.h"
#include "cXmlCDATASection_glue.h"
#include "cXmlDocument_glue.h"
#include "cXml_glue.h"
#endif

