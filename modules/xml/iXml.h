/*
 * Copyright (C) 2010 Ajax.org BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
namespace o3 {

    
    o3_iid(iXmlNodeList2, 0x17b83a81, 0x15c2, 0x4304, 0x9b, 0xcf, 0xb5, 0xe7, 0xde, 0xcb, 0xaa, 0x34);    
	o3_iid(iXmlNodeArray2, 0x1dede7dd, 0xcd4b, 0x4a2f, 0x94, 0xd3, 0xdd, 0xca, 0xa7, 0x84, 0x43, 0x84); 
    o3_iid(iXmlNamedNodeMap2, 0x155d1b4c, 0xcdce, 0x4968, 0x9b, 0x46, 0xa, 0x5d, 0xd3, 0x85, 0xa2, 0xdf);
	o3_iid(iXmlNode2, 0x1aac3ccb, 0x6394, 0x4dec, 0x91, 0x33, 0x49, 0x1c, 0xe, 0x8, 0x3f, 0xe3);
    o3_iid(iXmlElement2, 0x1d5c3325, 0x4567, 0x4d3f, 0x89, 0x3d, 0x30, 0x2c, 0xe0, 0xda, 0x6a, 0x6b);
	o3_iid(iXmlText2, 0x1df0bee1, 0xd05b, 0x4587, 0x98, 0x85, 0x6, 0xcd, 0x70, 0xf8, 0x39, 0xb6);
    o3_iid(iXmlCDATASection2,0x16e65a93, 0x36c4, 0x4be2, 0x9a, 0x2, 0x40, 0x60, 0xab, 0x1c, 0x38, 0xc3);
	o3_iid(iXmlComment2, 0x1d13412e, 0xf3e0, 0x4f85, 0x9b, 0x42, 0x6c, 0xc9, 0x18, 0x6c, 0x3a, 0x95);
	o3_iid(iXmlDocument2, 0x18508e87, 0xc49d, 0x4951, 0xba, 0x49, 0xb2, 0x6f, 0x26, 0xc2, 0xc0, 0x94);
    o3_iid(iXml2, 0x1e5245a9, 0x582b, 0x4e1a, 0x8c, 0x97, 0xb8, 0xc9, 0xbf, 0x1c, 0xd4, 0xcb);
    o3_iid(iXmlCharacterData2, 0x243a421d, 0xe895, 0x409b, 0xac, 0x55, 0xdb, 0x5e, 0xab, 0x14, 0x15, 0x89);
    o3_iid(iXmlAttr2, 0x1fc036d5, 0xf100, 0x4b2d, 0xa6, 0x88, 0xd0, 0x30, 0x2a, 0x5a, 0x84, 0x84);

    typedef tMap<xmlNodePtr, iXmlNode2*> NodeMap;

    struct iXml2 : iUnk {

    };

    struct iXmlAttr2 : iUnk {
            virtual Str name() = 0;
            virtual bool specified() = 0;
            virtual Str value() = 0;
            virtual void setValue(const char* value) = 0;
    };

    struct iXmlCDATASection2 : iUnk {

	};

	struct iXmlCharacterData2 : iUnk {
		virtual Str data() = 0;
		virtual void setData(const char* data) = 0;
		virtual int length() = 0;

        virtual Str substringData(int offset, int count, siEx* ex = 0) = 0;
		virtual void appendData(const char* arg) = 0;
		virtual void insertData(int offset, const char* arg, siEx* ex = 0) = 0;
		virtual void deleteData(int offset, int count, siEx* ex = 0) = 0;
		virtual void replaceData(int offset, int count, const char* arg, siEx* ex = 0) = 0;
	};

    struct iXmlComment2 : iUnk {

	};

    struct iXmlDocument2 : iUnk {
        virtual siXmlElement2 documentElement(iCtx* ctx) = 0;
        virtual siXmlElement2 createElement(iCtx* ctx, const char* tagName) = 0;
		// TODO: createDocumentFragment()
        virtual siXmlText2 createTextNode(iCtx* ctx, const char* data) = 0;
		virtual siXmlComment2 createComment(iCtx* ctx, const char* data) = 0;
		virtual siXmlCDATASection2 createCDATASection(iCtx* ctx, const char* data) = 0;
		// TODO: createProcessingInstruction()
        virtual siXmlAttr2 createAttribute(iCtx* ctx, const char* name) = 0;
		// TODO: createEntityReference()
        virtual siXmlNodeList2 getElementsByTagName(iCtx* ctx, const char* tagName) = 0;        
        virtual siXmlElement2 getElementById(iCtx* ctx, const char* id) = 0;        
        
        virtual xmlDocPtr docPtr() = 0;
		virtual const tMap<Str,Str>& nameSpaces() = 0;
    };

    struct iXmlElement2 : iUnk {
        virtual Str tagName() = 0;
        
        virtual Str getAttribute(iCtx* ctx, const char* name) = 0;
        virtual void setAttribute(iCtx* ctx, const char* name, const char* value) = 0;
        virtual void removeAttribute(iCtx* ctx, const char* name) = 0;
        
        virtual siXmlAttr2 getAttributeNode(iCtx* ctx, const char* name) = 0;
        virtual siXmlAttr2 setAttributeNode(iCtx* ctx, iXmlAttr2* new_attr, siEx* ex = 0) = 0;
        virtual siXmlAttr2 removeAttributeNode(iXmlAttr2* old_attr) = 0;
        
		virtual siXmlNodeList2 getElementsByTagName(const char* name) = 0;
        virtual siXmlElement2 getElementById(iCtx* ctx, const char* id) = 0;        
        virtual void normalize(iCtx* ctx) = 0;

        virtual xmlAttrPtr firstAttr() = 0;
    };

    struct iXmlNamedNodeMap2 : iUnk {
		virtual siXmlNode2 getNamedItem(iCtx* ctx, const char* name) = 0;
		virtual siXmlNode2 setNamedItem(iCtx* ctx, iXmlNode2* arg) = 0;
		virtual siXmlNode2 removeNamedItem(iCtx* ctx, const char* name) = 0;

		virtual siXmlNode2 item(iCtx* ctx, int index) = 0;
		virtual int length() = 0;
    };

    struct iXmlNode2 : iUnk {        

        enum Type {
            TYPE_ELEMENT = 1,
            TYPE_ATTRIBUTE = 2,
            TYPE_TEXT = 3,
            TYPE_CDATA_SECTION = 4,
            TYPE_COMMENT = 8,
            TYPE_DOCUMENT = 9
        };

        virtual Str nodeName() = 0;
        virtual Str nodeValue() = 0;
        virtual void setNodeValue(const char* value) = 0;
        virtual Type nodeType() = 0;
        
		virtual siXmlNode2 parentNode(iCtx* ctx) = 0;
        virtual siXmlNodeList2 childNodes() = 0;
        virtual siXmlNode2 firstChild(iCtx* ctx) = 0;
        virtual siXmlNode2 lastChild(iCtx* ctx) = 0;
        virtual siXmlNode2 previousSibling(iCtx* ctx) = 0;
        virtual siXmlNode2 nextSibling(iCtx* ctx) = 0;
        virtual siXmlNamedNodeMap2 attributes() = 0;
        virtual siXmlDocument2 ownerDocument(iCtx* ctx) = 0;
        
		virtual siXmlNode2 insertBefore(iCtx* ctx, iXmlNode2* ref_child, iXmlNode2* new_child, siEx* ex = 0) = 0;
        virtual siXmlNode2 replaceChild(iCtx* ctx, iXmlNode2* new_child, iXmlNode2* old_child, siEx* ex = 0) = 0;
        virtual siXmlNode2 removeChild(iXmlNode2* old_child, siEx* ex = 0) = 0;
        virtual siXmlNode2 appendChild(iCtx* ctx, iXmlNode2* new_child, siEx* ex = 0) = 0;
        
		virtual bool hasChildNodes() = 0;
        virtual siXmlNode2 cloneNode(iCtx* ctx, bool deep = false) = 0;     
        virtual siXmlNode2 ownerNode() = 0;
    };

    struct iXmlNodeArray2 : iUnk {
		virtual siXmlNode2 item(iCtx* ctx, int index) = 0;
		virtual int length() = 0;
    };

    struct iXmlNodeList2 : iUnk {
		virtual siXmlNode2 item(iCtx* ctx, int index) = 0;
		virtual int length(iCtx* ctx) = 0;
    };

    struct iXmlText2 : iUnk {
		virtual siXmlText2 splitText(int offset) = 0; 
	};

    siXmlNode2 wrapNode(iCtx* ctx, xmlNodePtr node, iXmlNode2* owner_node);
}
