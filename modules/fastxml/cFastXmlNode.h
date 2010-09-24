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
#ifndef J_C_XML_NODE_H
#define J_C_XML_NODE_H

namespace o3 {
  struct cFastXmlNode : cScr, iXmlNode {
		struct XmlStrWriter: pugi::xml_writer
		{
		  Str result;

		  virtual void write(const void* data, size_t size)
		  {
			  result += Str( (const char*)(data), size);
		  }
		};
 
		cFastXmlNode(iXmlDocument* doc, xml_node node, xml_attribute attr)
		   : m_node(node)
		   , m_attr(attr)
		   , m_doc(doc)
	   {
			o3_trace3 trace;
       }

        virtual ~cFastXmlNode() 
		{
            o3_trace3 trace;
        }

        o3_begin_class(cScr)            
            o3_add_iface(iXmlNode)
        o3_end_class();

        o3_glue_gen()

		o3_enum("XmlNodeType",
				ELEMENT = 1,
				ATTRIBUTE = 2,
				TEXT = 3,
				CDATA_SECTION = 4,
				COMMENT = 8,
				DOCUMENT = 9
		);

		xml_node		m_node;
		xml_attribute	m_attr;

		siXmlDocument	m_doc;

        o3_fun siXmlNode replaceNode(iXmlNode* new_child, siEx* ex = 0) 
		{
			o3_trace3 trace;
			if (!new_child)
				return siXmlNode();

			siXmlNode parent = this->parentNode();
			if (!parent)
				return siXmlNode();

			return parent->replaceChild(new_child, this, ex);
        }

        o3_get Str xml() 
		{
            o3_trace3 trace;
			if (m_attr)
				return Str();

			XmlStrWriter xmlStrWriter;
			m_node.print(xmlStrWriter,"", pugi::format_raw);

			return xmlStrWriter.result;
		}
                
        virtual o3_get Str nodeName() 
		{
            o3_trace3 trace;
			if (m_attr)
				return m_attr.name();
			else
				return m_node.name();
        }
        
        virtual o3_get Str nodeValue() 
		{
            o3_trace3 trace;
            if (m_attr)
				return m_attr.value();
			else {
				Str ret = m_node.value();
				if (!ret.size())
					ret = m_node.child_value();
				return ret;
			}
		}
        
        virtual o3_set void setNodeValue(const char* value)
		{
            o3_trace3 trace;
			if (m_attr)
				m_attr.set_value(value);
			else
				m_node.set_value(value);
        }
        
        virtual o3_get XmlNodeType nodeType() 
		{
            o3_trace3 trace;
            // !TODO:
			if (m_attr)
				return TYPE_ATTRIBUTE;

			switch(m_node.type()) {
					case node_element:
						return TYPE_ELEMENT;		
					case node_document:
						return TYPE_DOCUMENT;
					case node_cdata:
						return TYPE_CDATA_SECTION;	
					case node_comment:
						return TYPE_COMMENT;	
					case node_pcdata:
						return TYPE_TEXT;
					default:								
						return TYPE_ELEMENT;

			}
        }
        
        virtual o3_get siXmlNode parentNode() 
		{
            o3_trace3 trace;
			if (m_attr)
				return wrapXmlNode(ownerDocument(), m_node);
			else {
				xml_node p = m_node.parent();
				return (p != m_node) ?
					wrapXmlNode(ownerDocument(), p)
					: siXmlNode();
			}
		}
        
		virtual o3_get siXmlNodeList childNodes() 
		{
            o3_trace3 trace;
			if (m_attr)
				return siXmlNodeList();
			return o3_new(cFastXmlNodeList)(ownerDocument(), this);
		}
        
        virtual o3_get siXmlNode firstChild() 
		{
            o3_trace3 trace;
			if (m_attr)
				return siXmlNode();
			else
				return wrapXmlNode(ownerDocument(), m_node.first_child());     
        }
        
        virtual o3_get siXmlNode lastChild() 
		{
            o3_trace3 trace;
			if (m_attr)
				return siXmlNode();
			else
				return wrapXmlNode(ownerDocument(), m_node.last_child());
		}
        
        virtual o3_get siXmlNode previousSibling() 
		{
            o3_trace3 trace;
			if (m_attr)
				return wrapXmlNode(ownerDocument(), m_node,m_attr.previous_attribute());
			else
				return wrapXmlNode(ownerDocument(), m_node.previous_sibling());
        }
        
        virtual o3_get siXmlNode nextSibling() 
		{
            o3_trace3 trace;
			if (m_attr)
				return wrapXmlNode(ownerDocument(), m_node,m_attr.next_attribute());
			else
				return wrapXmlNode(ownerDocument(), m_node.next_sibling());        }
       
		virtual o3_get siXmlNamedNodeMap attributes() 
		{
            o3_trace3 trace;
			if (siXmlElement elem = siXmlElement(this)) {
				return o3_new(cFastXmlNamedNodeMap)(elem);
			}
			return siXmlNamedNodeMap();
		}
        
        virtual o3_get siXmlDocument ownerDocument() 
		{
            o3_trace3 trace;		
			return m_doc ? m_doc : siXmlDocument(this);
        }
        
        virtual o3_fun siXmlNode insertBefore(iXmlNode* new_child,
                iXmlNode* ref_child, siEx* ex = 0) 
		{
            o3_trace3 trace;			
			if (!new_child) {
				if (ex)
					*ex = o3_new(cEx) ("Node to insert is invalid.");
				return siXmlNode();
			}
			if (!ref_child)
				return appendChild(new_child);

			if (m_attr || !m_node)
				return siXmlNode();

			if (new_child->ownerDocument() != ownerDocument()) {
				if (ex)
					*ex = o3_new(cEx)("Document mismatch");
				return siXmlNode();
			}

			if (siXmlNode p = new_child->parentNode()) {
				siXmlNode p2 = this;
				while (p2) {
					if (p2->node() == new_child->node()) {
						if (ex)
							*ex = o3_new(cEx)("The node to insert is one of this node's ancestors");
						return siXmlNode();
					}
					p2 = p2->parentNode();	
				}
				p->removeChild(new_child);
			}

			if (new_child->attr() && ref_child->attr()) { 
				// attribute insertion
				xml_attribute inserted = m_node.insert_copy_before(
					new_child->attr(), ref_child->attr());

				if (inserted) {
					swapNode(new_child,m_node,inserted);
					return new_child;
				}

				return siXmlNode();							
			}
			else if (!new_child->attr() && !ref_child->attr()) {
				// node insertion
				xml_node inserted = m_node.insert_copy_before(
					new_child->node(), ref_child->node());

				if (inserted) {
					swapNode(new_child,inserted);
					return new_child;
				}
			}
			
			return siXmlNode();
		}
        
        virtual o3_fun siXmlNode replaceChild(iXmlNode* new_child,
                iXmlNode* old_child, siEx* ex = 0) 
		{
            o3_trace3 trace;
			if (!new_child || !old_child){
				if (ex)
					*ex = o3_new(cEx) ("Node to replace is invalid.");
				return siXmlNode();
			}

			if (new_child == old_child)
				return new_child;
	
			if (new_child->ownerDocument() != ownerDocument()) {
				if (ex)
					*ex = o3_new(cEx)("Document mismatch");
				return siXmlNode();
			}

			if (siXmlNode p = new_child->parentNode()) {
				siXmlNode p2 = this->parentNode();
				while (p2) {
					if (p2->node() == new_child->node()) {
						if (ex)
							*ex = o3_new(cEx)("The node to put is one of this node's ancestors");
						return siXmlNode();
					}
					p2 = p2->parentNode();	
				}
				p->removeChild(new_child);
			}

			insertBefore(new_child, old_child);
			return removeChild(old_child);				
        }
        
        virtual o3_fun siXmlNode removeChild(iXmlNode* child, siEx* ex = 0) 
		{
            o3_trace3 trace;
			if (!child) {
				if (ex)
					*ex = o3_new(cEx) ("Node to remove is invalid.");
				return siXmlNode();
			}
				
			if (m_attr)
				return siXmlNode();

			bool ret;
			if (child->attr() && child->node() == m_node)
				ret = m_node.remove_attribute(child->attr());
			else
				ret = m_node.remove_child(child->node());

			return ret ? siXmlNode(child) : siXmlNode();
        }
        
        virtual o3_fun siXmlNode appendChild(iXmlNode* new_child, siEx* ex = 0) 
		{
            o3_trace3 trace;


			if (!new_child){
				if (ex)
					*ex = o3_new(cEx) ("Node to append is invalid.");
				return siXmlNode();
			}
				

			if (new_child->ownerDocument() != ownerDocument()) {
				if (ex)
					*ex = o3_new(cEx)("Document mismatch");
				return siXmlNode();
			}

			if (siXmlNode p = new_child->parentNode()) {
				siXmlNode p2 = this;
				while (p2) {
					if (p2->node() == new_child->node()) {
						if (ex)
							*ex = o3_new(cEx)("The node to append is one of this node's ancestors");
						return siXmlNode();
					}
					p2 = p2->parentNode();	
				}
				p->removeChild(new_child);
			}

			if (m_attr || !m_node)
				return siXmlNode();

			siXmlNode ret;
			if (new_child->attr()) {
				// attribute
				xml_attribute attr = m_node.append_copy(new_child->attr());
				if (attr) {
					swapNode(new_child, m_node, attr);
					ret = new_child;
				}
			}
			else if (new_child->node()) {
				// node
				xml_node node = m_node.append_copy(new_child->node());
				swapNode(new_child, node);
				ret = new_child;	
			}

			return ret;
			
        }
        
        virtual o3_get bool hasChildNodes() 
		{
			// !TODO: implement
            o3_trace3 trace;
			if (m_attr)
				return false;
			return m_node.first_child();
		}
        
		virtual o3_fun siXmlNode cloneNode(bool deep) 
		{
			o3_trace3 trace;
			o3_assert(deep);
			if (!m_node && !m_attr 
				|| m_node.type() == node_document) 
				return siXmlNode();

			if (m_attr) {
				xml_node tmp = ownerDocument()->doc().append_child(node_element);
				xml_attribute attr = tmp.append_copy(m_attr);
				tmp.remove_attribute(attr);
				ownerDocument()->doc().remove_child(tmp);
				return wrapXmlNode(ownerDocument(), xml_node(), attr);
			}

			else 
				return wrapXmlNode(ownerDocument(), m_node.clone(deep));
		}

		o3_get Str namespaceURI() 
		{
			o3_trace3 trace;
			o3_assert(false);
			return Str();
		}

		virtual xml_node& node()
		{
			return m_node;
		}

		virtual xml_attribute& attr()
		{
			return m_attr;
		}

		void swapNode( iXmlNode* target, xml_node new_node, xml_attribute new_attr=xml_attribute() ) 
		{
			o3_assert(target);			
			if (xml_attribute attr = target->attr()) {
				iWeak* ptr = (iWeak*) attr.get_user_ptr();
				ptr->release();
				siWeak new_ref = siXmlNode(target);
				new_ref->addRef();
				attr.set_user_ptr(new_ref.ptr(), &destroyNode);
			} 
			else if (xml_node node = target->node()){
				iWeak* ptr = (iWeak*) node.get_user_ptr();
				ptr->release();
				siWeak new_ref = siXmlNode(target);
				new_ref->addRef();
				node.set_user_ptr(new_ref.ptr(), &destroyNode);
			}
			cFastXmlNode* ctarget = (cFastXmlNode*) target;
			ctarget->m_node = new_node;
			ctarget->m_attr = new_attr;
		}
/*
		const char_t* namespace_uri(const xml_node& node)
		{
			const char_t* pos = find_char(node.name(), ':');

			string_t ns = PUGIXML_TEXT("xmlns");

			if (pos)
			{
				ns += ':';
				ns.append(node.name(), pos);
			}

			xml_node p = node;

			while (p)
			{
				xml_attribute a = p.attribute(ns.c_str());

				if (a) return a.value();

				p = p.parent();
			}

			return PUGIXML_TEXT("");
		}
*/
    };
}

#endif // J_C_XML_NODE_H
