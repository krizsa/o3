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
#ifndef J_C_XML_H
#define J_C_XML_H

namespace o3 {

    struct cFastXml: cScr, iXml {

        o3_begin_class(cScr)
            o3_add_iface(iXml)
        o3_end_class();

        o3_glue_gen()


		static o3_ext("cO3") o3_get siXml fastxml(iCtx* ctx)
		{
			Var val = ctx->value("xml");
			siXml xml = val.toScr();
			if (xml)
				return xml;

			val = siScr(o3_new(cFastXml)());
			ctx->setValue("xml", val);

			return val.toScr();
		}
        
        o3_fun siXmlNode parseFromString(iCtx* ctx, const char* str, const char* contentType = "text/xml") 
		{
            o3_assert(str);
            o3_assert(contentType);
			// create a new document
			siXmlDocument doc = o3_new(cFastXmlDocument)();
			// parse the xml string
			doc->doc().load(str, pugi::parse_default | pugi::parse_comments | pugi::parse_ws_pcdata);
			siWeak wref = siXmlNode(doc);
			wref->addRef();
			doc->doc().set_user_ptr(wref.ptr(), &destroyNode);
			return doc;          
        }

    };

	siXmlNode wrapXmlNode(iXmlDocument* doc, xml_node node, xml_attribute attr) 
	{
		siXmlNode ret;
		siWeak ref;	

		if (attr) {
			if (iWeak* ptr = (iWeak*) attr.get_user_ptr()) 
				ret = siWeak(ptr);
			if (!ret) {
				ret = o3_new(cFastXmlAttr)(doc, node, attr);
				ref = ret.ptr();
				ref->addRef();
				attr.set_user_ptr(ref.ptr(), &destroyNode);
			}
			
		}
		else if (node) {
			if (iWeak* ptr = (iWeak*) node.get_user_ptr()) 
				ret = siWeak(ptr);
			if (!ret) {
				switch (node.type()) {
					case node_element:
						ret = o3_new(cFastXmlElement)(doc, node, attr);
						break;
					case node_document:
							o3_assert(false);
							break;
					case node_cdata:
						ret = o3_new(cFastXmlCDATASection)(doc, node, attr);
						break;	
					case node_comment:
						ret = o3_new(cFastXmlComment)(doc, node, attr);
						break;	
					case node_pcdata:
						ret = o3_new(cFastXmlText)(doc, node, attr);
						break;	
					
					default:								
						o3_assert(false);
				}
				if (ret) {
					ref = ret.ptr();
					ref->addRef();
					node.set_user_ptr(ref.ptr(), &destroyNode);
				}
			}
		}	

		return ret;
	}

	void destroyNode(void* ref) {
		if (iWeak* ptr = (iWeak*) ref) {
			ptr->release();
		}	
	}
          
}

#endif // J_C_XML_H
