//============================================================================
// Name        : squishyXML.cpp
// Author      : Jan Schiefer
// Version     : 0.1
// Copyright   : GNU Lesser General Public License (LGPL)
// Description : A dead easy libxml2 wrapper in C++/RAII-style.
//============================================================================

#include <iostream>
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

squishyXMLDocument::squishyXMLDocument( squishyXMLContext &context, std::string_view filename, std::string_view encoding, int options ) {

	ptr = xmlCtxtReadFile(context.ptr, std::string(filename).c_str() ,std::string(encoding).c_str(), options);

}

squishyXMLDocument::squishyXMLDocument( std::string_view version ) {

	if(!version.empty()) ptr = xmlNewDoc( (const xmlChar *) version.data());
	else ptr = xmlNewDoc( NULL );

}

squishyXMLDocument::~squishyXMLDocument() {	if( ptr ) xmlFreeDoc(ptr); }

void squishyXMLDocument::setRootElement( xmlNode *rootNode ) {

    if ( ptr == NULL ) throw std::runtime_error( "[ERROR] Cannot set XML root node in uninitialized document.\n" );

	xmlDocSetRootElement(ptr, rootNode );

}

xmlNode *squishyXMLDocument::getRootElement() {

    if ( ptr == NULL ) throw std::runtime_error( "[ERROR] Cannot get XML root node of uninitialized document.\n" );

	return xmlDocGetRootElement( ptr );

}

void squishyXMLDocument::printDocToString( std::string &result, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl ) {

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

			xmlNode *rootNode = getRootElement();

			if(outbuf && rootNode) {
				xmlNodeDumpOutput( outbuf, ptr, rootNode, 0, format, xmlEnc.data());
				xmlOutputBufferFlush(outbuf);

				if( xmlOutputBufferGetSize(outbuf) > 0 ) result.assign( (char *)xmlOutputBufferGetContent(outbuf) );

			}

			xmlOutputBufferClose(outbuf);

		}

	}

}

squishyXMLNodeUtil::squishyXMLNodeUtil() { }

squishyXMLNodeUtil::~squishyXMLNodeUtil() { }

xmlNode *squishyXMLNodeUtil::createNewNode( xmlNs *n_space, std::string_view name ) {

    if ( name.empty() ) throw std::runtime_error( "[ERROR] Cannot create XML node without name.\n" );

	return xmlNewNode( n_space, (const unsigned char *)name.data() );
}

void squishyXMLNodeUtil::unlinkNode ( xmlNode *node, bool freeData ) {

	if (node) {

		xmlUnlinkNode(node);
		if (freeData) xmlFreeNode(node);

	}
}


xmlNode *squishyXMLNodeUtil::findSingleNodeByName( std::string_view nodeName, xmlNode *startNode ) {

	if( startNode == NULL ) throw std::runtime_error( "[ERROR] findSingleNodeByName tried on NULL startNode.\n" );

	if( nodeName.empty() ) throw std::runtime_error( "[ERROR] Cannot find XML node without name.\n" );;

	for (xmlNode *cur_node = startNode; cur_node; cur_node = cur_node->next) {

		if (cur_node->type == XML_ELEMENT_NODE && nodeName.compare((const char *)cur_node->name) == 0)
				return cur_node;
	}

	return NULL;
}

xmlNode *squishyXMLNodeUtil::findSingleChildNodeByName( std::string_view nodeName, xmlNode *startNode ) {

	if( startNode == NULL ) throw std::runtime_error( "[ERROR] findSingleChildNodeByName tried on NULL startNode.\n" );

	if( nodeName.empty() ) throw std::runtime_error( "[ERROR] Cannot find child XML node without name.\n" );

	if( startNode->children == NULL ) return NULL;

	return findSingleNodeByName( nodeName, startNode->children );

}

void squishyXMLNodeUtil::findNodesByName( std::string_view nodeName, xmlNode *startNode, std::vector<xmlNode *>  &results ) {

	if( startNode == NULL ) throw std::runtime_error( "[ERROR] findNodesByName tried on NULL startNode.\n" );

	results.clear();

	for (xmlNode *cur_node = startNode; cur_node; cur_node = cur_node->next) {

		if (cur_node->type == XML_ELEMENT_NODE && nodeName.compare((const char *)cur_node->name) == 0)
				results.push_back(cur_node);

	}
}

void squishyXMLNodeUtil::findChildNodesByName( std::string_view nodeName, xmlNode *startNode, std::vector<xmlNode *>  &results ) {

	if( startNode == NULL ) throw std::runtime_error( "[ERROR] findChildNodesByName tried on NULL startNode.\n" );
	if( startNode->children == NULL ) 	results.clear();
	else findNodesByName(nodeName, startNode->children, results);
}

bool squishyXMLNodeUtil::getNodeContent( const xmlNode *node, std::string &result ) {

	result.clear();

	if( node ) {

		xmlChar *content = xmlNodeGetContent ( node );

		if( content ) {
			result =  (char *)content;
			xmlFree ( content );
		}

		return true;
	}

	return false;

}

bool squishyXMLNodeUtil::getNodeProperty( const xmlNode *node, std::string_view name, std::string &result ) {

	result.clear();

	if( node && !name.empty() ) {

		xmlChar *prop = xmlGetProp ( node, (const xmlChar *)name.data() );

		if( prop ) {
			result =  (char *)prop;
			xmlFree ( prop );
		}

		return true;
	}

	return false;


}

void squishyXMLNodeUtil::getNodeProperties( const xmlNode *node, std::unordered_map <std::string, std::string> &result ) {

	result.clear();

	if( node ) {

		xmlAttr *attribute = node->properties;
		xmlChar *value;

			while(attribute && attribute->name && attribute->children) {
				value = xmlNodeListGetString(node->doc, attribute->children, 1);
				result.insert( std::make_pair( std::string((char *)attribute->name), std::string((char *) value) ) );
				xmlFree(value);
				attribute = attribute->next;
		    }

	}

}

void squishyXMLNodeUtil::setNodeContent( xmlNode *node,  std::string_view content ) {
	if( node == NULL ) throw std::runtime_error( "[ERROR] Unable to st content on NULL node.\n" );

	xmlChar *escapedContents = xmlEncodeEntitiesReentrant( node->doc, (xmlChar *)content.data());

	xmlNodeSetContent ( node, escapedContents );

	xmlFree( escapedContents );
}

void squishyXMLNodeUtil::changeNodeName ( xmlNode *node, std::string_view name ) {

    if ( node == NULL ) throw std::runtime_error( "[ERROR] Cannot set XML node name for an undefined node.\n" );

    if ( name.empty() ) throw std::runtime_error( "[ERROR] Cannot set XML node name to an empty value.\n" );

    xmlNodeSetName ( node, (xmlChar *)name.data() );

}

void squishyXMLNodeUtil::setNodeProperty( xmlNode *node, std::string_view key, std::string_view value ) {

	if ( node == NULL ) throw std::runtime_error( "[ERROR] Cannot set property for an undefined node.\n" );

    if ( key.empty() ) throw std::runtime_error( "[ERROR] Cannot set an unnamed property.\n" );

	xmlSetProp (node, (xmlChar *)key.data(), (xmlChar *) value.data() );

}


void squishyXMLNodeUtil::setNodeProperties( xmlNode *node, const std::unordered_map <std::string, std::string> &properties ) {

    if ( node == NULL ) throw std::runtime_error( "[ERROR] Cannot set properties for an undefined node.\n" );

	for (const auto& [key, value] : properties) {

		if(!key.empty()) xmlSetProp (node, (xmlChar *)key.c_str(), (xmlChar *) value.c_str() );

	}

}


bool squishyXMLNodeUtil::addChildNode( xmlNode *parent, xmlNode *child ) {

	return ( xmlAddChildList( parent, child ) != NULL );

}


