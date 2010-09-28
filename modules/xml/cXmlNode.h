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
#ifndef J_C_XML_NODE2_H
#define J_C_XML_NODE2_H


namespace o3 {
  struct cXmlNode : cScr, iXmlNode2 {
       cXmlNode(xmlNodePtr node, iXmlNode2* owner_node, NodeMap* node_map) {
            m_node = node;
            m_owner_node = owner_node;
            m_node_map = node_map;
            (*m_node_map)[m_node] = this;
			//printf("create %p %p\n", m_node, this);
        }

        virtual ~cXmlNode() {
            o3_trace3 trace;
			//printf("destroy %p %p\n", m_node, this);
			if (m_node)
				m_node_map->remove(m_node);

            if (!m_owner_node && m_node){                                
				//printf("remove %p\n", m_node);
				xmlFreeNode(m_node);
            }
        }

        o3_begin_class(cScr)            
            o3_add_iface(iXmlNode2)
        o3_end_class();

        o3_glue_gen()

		o3_enum("Type",
				ELEMENT = 1,
				ATTRIBUTE = 2,
				TEXT = 3,
				CDATA_SECTION = 4,
				COMMENT = 8,
				DOCUMENT = 9
		);

        o3_fun siXmlNode2 replaceNode(iCtx* ctx, iXmlNode2* new_child, siEx* ex = 0) {
			if (!new_child)
				return siXmlNode2();
            return parentNode(ctx)->replaceChild(ctx, new_child, this, ex);
        }

        o3_get Str xml() {
            o3_trace3 trace;

            xmlBufferPtr buf = xmlBufferCreate();
            xmlNodeDump(buf, m_node->doc, m_node, 0, 0);
            Str ret = Str((char*)buf->content);
            xmlBufferFree(buf);
            return ret;
        }
                
        virtual o3_get Str nodeName() {
            o3_trace3 trace;
            return Str((const char*) m_node->name);
        }
        
        virtual o3_get Str nodeValue() {
            o3_trace3 trace;
            char* value = (char*) xmlNodeGetContent(m_node);
            Str result(value);
            //! TODO xmlFree is not a function
            //xmlFree(value);
            return result;
        }
        
        virtual o3_set void setNodeValue(const char* value) {
            o3_trace3 trace;
            xmlNodeSetContent(m_node, BAD_CAST value);
        }
        
        virtual o3_get Type nodeType() {
            o3_trace3 trace;
            return (Type) m_node->type;
        }
        
        virtual o3_get siXmlNode2 parentNode(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, m_node->parent, m_owner_node ? m_owner_node : this);
        }
        
		virtual o3_get siXmlNodeList2 childNodes() {
            o3_trace3 trace;
            return siXmlNodeList2(o3_new(cXmlNodeList)(this));
		}
        
        virtual o3_get siXmlNode2 firstChild(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, m_node->children, m_owner_node ? m_owner_node : this);
        }
        
        virtual o3_get siXmlNode2 lastChild(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, m_node->last, m_owner_node ? m_owner_node : this);
        }
        
        virtual o3_get siXmlNode2 previousSibling(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, m_node->prev, m_owner_node ? m_owner_node : this);
        }
        
        virtual o3_get siXmlNode2 nextSibling(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, m_node->next, m_owner_node ? m_owner_node : this);
        }
       
		virtual o3_get siXmlNamedNodeMap2 attributes() {
			o3_trace3 trace;
			siXmlElement2 elem(this);
			if (!elem)
				return elem;

			return o3_new(cXmlNamedNodeMap)(elem);
		}
        
        virtual o3_get siXmlDocument2 ownerDocument(iCtx* ctx) {
            o3_trace3 trace;
            return wrapNode(ctx, (xmlNodePtr) m_node->doc, m_owner_node ? m_owner_node : this);
        }
        
        virtual o3_fun siXmlNode2 insertBefore(iCtx* ctx, iXmlNode2* new_child,
                iXmlNode2* ref_child, siEx* ex = 0)
        {
            o3_trace3 trace;
            cXmlNode* new_child2 = (cXmlNode*) new_child;
            cXmlNode* ref_child2 = (cXmlNode*) ref_child;

            // Check arguments
            if (!new_child) {
                if (ex)
                    *ex = o3_new(cEx)("Invalid argument");
                return 0;
            }

            // Make sure both nodes are part of the same document
            if (new_child2->m_node->doc != m_node->doc) {
                if (ex)
                    *ex = o3_new(cEx)("Document mismatch");
                return 0;
            }

            // Check if node is to be inserted is not a descendant of itself
            xmlNodePtr node = m_node;
            do
                if (node == new_child2->m_node) {
                    if (ex)
                        *ex = o3_new(cEx)("Cannot insert node as descendant of itself");
                    return 0;
                }
            while (node = node->parent);
            
			siXmlNode2 removed;
			if (siXmlNode2 parent = new_child->parentNode(ctx)) {
				removed = parent->removeChild(new_child);
				new_child = removed.ptr();
				new_child2 = (cXmlNode*) new_child;
			}

            new_child2->m_owner_node = m_owner_node ? m_owner_node : this;
            if (ref_child2)
                xmlAddPrevSibling(ref_child2->m_node, new_child2->m_node);
            else
                xmlAddChild(m_node, new_child2->m_node);
            return new_child2;
        }
        
        virtual o3_fun siXmlNode2 replaceChild(iCtx* ctx, iXmlNode2* new_child,
                iXmlNode2* old_child, siEx* ex = 0) {
            o3_trace3 trace;
			if (!new_child || !new_child) {
				return siXmlNode2();
            }

            cXmlNode* new_child2 = (cXmlNode*) new_child;
            cXmlNode* old_child2 = (cXmlNode*) old_child;

			// Make sure both nodes are part of the same document
			if (new_child2->m_node->doc != m_node->doc) {
				if (ex)
					*ex = o3_new(cEx)("Document mismatch");
				return 0;
			}

			siXmlNode2 removed;
			if (siXmlNode2 parent = new_child->parentNode(ctx)) {
				removed = parent->removeChild(new_child);
				new_child = removed.ptr();
				new_child2 = (cXmlNode*) new_child;
			}

            new_child2->m_owner_node = m_owner_node ? m_owner_node : this;
            old_child2->m_owner_node = 0;
            xmlReplaceNode(old_child2->m_node, new_child2->m_node);
            
			//xmlDocDump(stdout, m_node->doc);
			
			return old_child2;
        }
        
        virtual o3_fun siXmlNode2 removeChild(iXmlNode2* old_child, siEx* ex = 0) {
            o3_trace3 trace;

            // Check arguments
            if (!old_child) {
                if (ex)
                    *ex = o3_new(cEx)("Invalid argument");
                return 0;
            }

            cXmlNode* old_child2 = (cXmlNode*) (old_child);


            old_child2->m_owner_node = 0;
            xmlUnlinkNode(old_child2->m_node);
			//xmlDocDump(stdout, m_node->doc);

			return old_child2;
        }
        
        virtual o3_fun siXmlNode2 appendChild(iCtx* ctx, iXmlNode2* new_child,
                                             siEx* ex = 0) {
            o3_trace3 trace;

			if (!new_child)
				return siXmlNode2();


            cXmlNode* new_child2 = (cXmlNode*) new_child;

            // Make sure both nodes are part of the same document
            if (new_child2->m_node->doc != m_node->doc) {
                if (ex)
                    *ex = o3_new(cEx)("Document mismatch");
                return 0;
            }


            // Check if node is to be inserted is not a descendant of itself
            xmlNodePtr node = m_node;
            do
                if (node == new_child2->m_node) {
                    if (ex)
                        *ex = o3_new(cEx)("Cannot append node as descendant of itself");
                    return 0;
                }
            while (node = node->parent);

			siXmlNode2 removed;
			if (siXmlNode2 parent = new_child->parentNode(ctx)) {
				removed = parent->removeChild(new_child);
				new_child = removed.ptr();
				new_child2 = (cXmlNode*) new_child;
			}

            new_child2->m_owner_node = m_owner_node ? m_owner_node : this;
            siXmlAttr2 attr;
			if ((attr = new_child).valid()) {
				siXmlElement2(this)->removeAttribute(ctx, attr->name());
			}
			
			xmlAddChild(m_node, new_child2->m_node);
            return new_child2;
        }
        
        virtual o3_get bool hasChildNodes() {
            o3_trace3 trace;
            return m_node->children != 0;
        }
        
		virtual o3_fun siXmlNode2 cloneNode(iCtx* ctx, bool deep) {
            o3_trace3 trace;
			xmlNodePtr root;
            
            if (deep)
                root = xmlCopyNode(m_node, 1);
            else
                root = xmlCopyNode(m_node, 2);

            return wrapNode(ctx, root, 0);
		}

		o3_get Str namespaceURI() 
		{
			if (!m_node)
				return "";
			
			xmlNs* ns = m_node->ns;
			if (!ns)
				return "";

			return Str((const char*) ns->href);
		}

        virtual siXmlNode2 ownerNode()
        {            
            return m_owner_node;
        }

        xmlNodePtr m_node;
        siXmlNode2  m_owner_node;
        NodeMap*   m_node_map; 
        bool       m_is_owner;
    };
}

#endif // J_C_XML_NODE_H
