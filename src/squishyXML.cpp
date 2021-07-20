//============================================================================
// Name        : squishyXML.cpp
// Author      : Jan Schiefer
// Version     : 0.1
// Copyright   : GNU Lesser General Public License (LGPL)
// Description : A dead easy libxml2 wrapper in C++/RAII-style.
//============================================================================

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include "../include/squishyXML.hpp"

using namespace std;

squishyXMLParser::squishyXMLParser() { LIBXML_TEST_VERSION }

squishyXMLParser::~squishyXMLParser() { xmlCleanupParser(); }

squishyXMLContext::squishyXMLContext() {

	ptr = xmlNewParserCtxt();

    if (ptr == NULL) throw std::runtime_error( "[FATAL] Failed to allocate XML parser context\n" );

}

squishyXMLContext::~squishyXMLContext() {	if(ptr) xmlFreeParserCtxt(ptr); }

squishyXMLNode::squishyXMLNode() { }

squishyXMLNode::squishyXMLNode( xmlNs *n_space, std::string_view name ) {

    if ( name.empty() ) throw std::runtime_error( "[ERROR] Cannot create XML node without name.\n" );

	ptr = xmlNewNode( n_space, (const unsigned char *)name.data() );

	doUnlink = true;
}

squishyXMLNode::~squishyXMLNode() {

	if(doUnlink) unlinkNode(true);

}

void squishyXMLNode::unlinkNode ( bool freeNode ) {

	if (ptr) {

		xmlUnlinkNode(ptr);

		if (freeNode) xmlFreeNode(ptr);
		ptr = NULL;
		doUnlink = false;

	}
}

void squishyXMLNode::setNodePointer( xmlNode *node ) {

	if(doUnlink) unlinkNode(true);

	if(node) ptr = node;

}

bool squishyXMLNode::findSingleNodeByName( std::string_view nodeName, squishyXMLNode &result, bool searchChildren ) {

	if( ptr == NULL ) return false;

	if( searchChildren && ptr->children == NULL ) return false;

	if( nodeName.empty() ) return false;

	for (xmlNode *cur_node = ( (searchChildren) ? (ptr->children) : (ptr) ) ; cur_node; cur_node = cur_node->next) {

		if (cur_node->type == XML_ELEMENT_NODE && nodeName.compare((const char *)cur_node->name) == 0) {

			squishyXMLNode ret;

			ret.setNodePointer(cur_node);

			result = ret;

			return true;

		}

	}

	return false;

}

bool squishyXMLNode::findNodesByName( std::string_view nodeName, std::vector<squishyXMLNode> &results, bool searchChildren ) {

	if( ptr == NULL ) return false;

	if( searchChildren && ptr->children == NULL ) return false;

	if( nodeName.empty() ) return false;

	results.clear();

	for (xmlNode *cur_node = ( (searchChildren) ? (ptr->children) : (ptr)); cur_node; cur_node = cur_node->next) {

		if (cur_node->type == XML_ELEMENT_NODE && nodeName.compare((const char *)cur_node->name) == 0) {

			squishyXMLNode temp;

			temp.setNodePointer(cur_node);

			results.push_back(temp);

		}

	}

	return !results.empty();
}

bool squishyXMLNode::getNodeContent( std::string &result ) {

	result.clear();

	if( ptr ) {

		xmlChar *content = xmlNodeGetContent ( ptr );

		if( content ) {
			result =  (char *)content;
			xmlFree ( content );

			return true;
		}

	}

	return false;

}

bool squishyXMLNode::getNodeProperty( std::string_view name, std::string &result ) {

	result.clear();

	if( ptr && !name.empty() ) {

		xmlChar *prop = xmlGetProp ( ptr, (const xmlChar *)name.data() );

		if( prop ) {
			result =  (char *)prop;
			xmlFree ( prop );

			return true;
		}

	}

	return false;

}

bool squishyXMLNode::getNodeProperties( std::unordered_map <std::string, std::string> &result ) {

	result.clear();

	if( ptr ) {

		xmlAttr *attribute = ptr->properties;
		xmlChar *value = NULL;

			while(attribute && attribute->name && attribute->children) {
				value = xmlNodeListGetString(ptr->doc, attribute->children, 1);
				result.insert( std::make_pair( std::string((char *)attribute->name), std::string((char *) value) ) );
				xmlFree(value);
				attribute = attribute->next;
		    }

	}

	return !result.empty();

}

bool squishyXMLNode::setNodeContent(  std::string_view content ) {
	if( ptr == NULL ) return false;

	xmlChar *escapedContents = xmlEncodeEntitiesReentrant( ptr->doc, (xmlChar *)content.data());

	xmlNodeSetContent ( ptr, escapedContents );

	xmlFree( escapedContents );

	return true;
}

bool squishyXMLNode::setNodeProperty( std::string_view key, std::string_view value ) {

	if ( ptr == NULL ) return false;

    if ( key.empty() ) return false;

	xmlSetProp (ptr, (xmlChar *)key.data(), (xmlChar *) value.data() );

	return true;

}


bool squishyXMLNode::setNodeProperties( const std::unordered_map <std::string, std::string> &properties ) {

    if ( ptr == NULL ) return false;

    bool retval = false;

	for (const auto& [key, value] : properties) {

		if(!key.empty()) {
			xmlSetProp (ptr, (xmlChar *)key.c_str(), (xmlChar *) value.c_str() );
			retval = true;
		}

	}

	return retval;
}

bool squishyXMLNode::changeNodeName ( std::string_view name ) {

    if ( ptr == NULL ) return false;

    if ( name.empty() ) return false;

    xmlNodeSetName ( ptr, (xmlChar *)name.data() );

    return true;

}

bool squishyXMLNode::addChildNode(  squishyXMLNode &child ) {

	return ( xmlAddChildList( ptr, child.ptr ) != NULL );

}

squishyXMLDocument::squishyXMLDocument( squishyXMLContext &context, std::string_view filename, std::string_view encoding, int options ) {

	ptr = xmlCtxtReadFile(context.ptr, std::string(filename).c_str() ,std::string(encoding).c_str(), options);

}

squishyXMLDocument::squishyXMLDocument( std::string_view version ) {

	if(!version.empty()) ptr = xmlNewDoc( (const xmlChar *) version.data());
	else ptr = xmlNewDoc( NULL );

}

squishyXMLDocument::~squishyXMLDocument() {	if( ptr ) xmlFreeDoc(ptr); }

bool squishyXMLDocument::setRootElement( squishyXMLNode &node ) {

    if ( ptr == NULL ) return false;

	xmlDocSetRootElement(ptr, node.ptr );

	return true;

}

bool squishyXMLDocument::getRootElement( squishyXMLNode &node ) {

    if ( ptr == NULL ) return false;

    node.setNodePointer( xmlDocGetRootElement( ptr ) );

	return true;

}

bool squishyXMLDocument::printDocToString( std::string &result, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl ) {

	result.clear();

	if( ptr ) {

		std::string_view xmlEnc = ( (encoding.empty()) ? "UTF-8" : encoding.data() );
		int format = ( (addFormattingSpaces)? 1 : 0 );

		if( withXMLDecl ) {
			xmlChar *sequence = NULL;
			int size = 0;

			xmlDocDumpFormatMemoryEnc( ptr, &sequence, &size, xmlEnc.data(), format );

			if ( size > 0 )	result.assign( ( char *) sequence );

			if( sequence ) xmlFree( sequence );

		}

		else {
			xmlOutputBuffer *outbuf = xmlAllocOutputBuffer(NULL);

			squishyXMLNode rootNode;

			getRootElement( rootNode );

			if(outbuf && rootNode.ptr) {
				xmlNodeDumpOutput( outbuf, ptr, rootNode.ptr, 0, format, xmlEnc.data());
				xmlOutputBufferFlush(outbuf);

				if( xmlOutputBufferGetSize(outbuf) > 0 ) result.assign( (char *)xmlOutputBufferGetContent(outbuf) );

			}

			xmlOutputBufferClose(outbuf);

		}

	}

	return !result.empty();
}

bool squishyXMLDocument::printDocToFile( std::string_view filename, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl ) {

	std::string buffer;

	if( !printDocToString( buffer, encoding , addFormattingSpaces, withXMLDecl ) ) return false;

	std::ofstream outfile(filename.data());

	if( !outfile.is_open() || !outfile.good() ) return false;

	outfile << buffer;

	return outfile.good();

}

