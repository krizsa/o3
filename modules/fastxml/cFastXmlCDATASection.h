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
#ifndef J_C_XML_CDATA_SECTION_H
#define J_C_XML_CDATA_SECTION_H

namespace o3 {
	struct cFastXmlCDATASection : cFastXmlText, iXmlCDATASection {
        cFastXmlCDATASection(iXmlDocument* doc, xml_node node, xml_attribute attr) 
            : cFastXmlText(doc,node,attr) 
		{
                
        }

		virtual ~cFastXmlCDATASection()
		{

		}

		o3_begin_class(cFastXmlText)
			o3_add_iface(iXmlCDATASection)
		o3_end_class();

        o3_glue_gen()
	};
}

#endif // J_C_XML_CDATA_SECTION_H
