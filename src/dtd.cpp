#include <string_view>
#include <libxml/relaxng.h>
#include "dtd.h"

namespace
{
    constexpr std::string_view NCX = R"xml(<!-- NCX 2005-1 DTD  2005-06-26
file: ncx-2005-1.dtd

  Authors: Mark Hakkinen, George Kerscher, Tom McLaughlin, James Pritchett, and Michael Moodie
  Change list:
  2002-02-12 M. Moodie. Changed content model of navLabel element to eliminate ambiguity.
  2002-02-27 M. Moodie. Grammatical changes suggested by editor.
  2004-03-31 J. Pritchett.  Various changes per the 2004 change list:
            - Changed internal version numbers from 1.1.0 to 1.2.0
            - Made audio clipBegin/clipEnd mandatory (change #10)
            - Dropped value attribute from navPoint (change #11)
            - Replaced lang attribute with xml:lang (change #12)
            - Added <pageList> and <pageTarget> elements (change #48)
            - Dropped onFocus and onBlur attributes from navPoint and navTarget (change #49)
            - Added <img> to content models of docTitle and docAuthor (change #50)
            - Removed reference to pages in description of navList (change #52)
            - Added <navInfo> element (change #53)
            - Added default namespace attribute to description of <ncx> (change #L8)
            - Removed pageRef and mapRef attributes
  2004-04-05 J. Pritchett.  Changes after feedback from MM and MG to 2004-03-31 version
            - Changed internal version numbers from 1.2.0 to 1.1.2 (per MM e-mail of 3/31)
            - Changed system identifier to use z3986/2004 as path instead of z3986/v100 (per 3/31 con call)
            - Added class attribute to both pageTarget and pageList (per MG e-mail of 4/1)
            - Added comment text describing value attribute for pageTarget and navTarget (per MM e-mail of 3/31)
            - Changed declaration of type attribute on pageTarget to enumerate allowed values
            - Added playOrder attribute to navPoint, navTarget, and pageTarget (per Lloyd's proposal)
2004-04-05 T. McLaughlin. In description of smilCustomTest, added id and defaultState are to be copied.
Version update to 1.1.3.
2004-05-14 T. McLaughlin. Reinstated override attribute to be copied also. Added bookStruct attribute
and enum list to smilCustomTest. Update to 1.1.4.
Revised, 4/5/2004:  Changed version to 1.1.2
Revised, 4/5/2004:  Changed system identifier to use '2004' path
Revised, 4/5/2004:  TM, Changed version to 1.1.3
Revised, 5/14/2004:  TM, Changed version to 1.1.4
2004-07-07 M. Moodie Updated version to 1.2.0 everywhere but at top, where version was set to 1.1.5.
2004-09-15 M. Moodie.  Changed uri to URI throughout.  Set version to 1.1.6.
2004-09-16 M. Moodie.  Changed version to 1.2.0
2005-06-26 M. Gylling. Changed pid, sid, ns uri, and filename for Z3986-2005

  Description:

  NCX (Navigation Control for XML applications) is a generalized navigation definition DTD for application
to Digital Talking Books, eBooks, and general web content models.
This DTD is an XML application that layers navigation functionality on top of SMIL 2.0  content.

  The NCX defines a navigation path/model that may be applied upon existing publications,
without modification of the existing publication source, so long as the navigation targets within
the source publication can be directly referenced via a URI.

The following identifiers apply to this DTD:
  "-//NISO//DTD ncx 2005-1//EN"
  "http://www.daisy.org/z3986/2005/ncx-2005-1.dtd"
-->
<!-- Basic Entities -->
<!--ENTITY % i18n
  "xml:lang    NMTOKEN    #IMPLIED
  dir    (ltr|rtl)  #IMPLIED" -->
<!--ENTITY % SMILtimeVal  "CDATA" -->
<!--ENTITY % URI    "CDATA" -->
<!--ENTITY % script  "CDATA" -->
<!-- ELEMENTS -->
<!-- Top Level NCX Container. -->
<!-- Revised, 3/31/2004:  Added pageList to content model -->
<!--ELEMENT ncx (head, docTitle, docAuthor*, navMap, pageList?, navList*)-->
<!-- Revised, 4/5/2004:  Changed version to 1.1.2 -->
<!-- Revised 3/29/2004:  Added xmlns -->
<!-- Revised, 4/5/2004:  TM, Changed version to 1.1.3 -->
<!-- Revised, 5/14/2004:  TM, Changed version to 1.1.4 -->
<!-- Revised, 7/7/2004:  MM, Changed version to 1.2.0 -->
<!--ATTLIST ncx
  version     CDATA     #FIXED "2005-1"
  xmlns       %URI;     #FIXED "http://www.daisy.org/z3986/2005/ncx/"
  %i18n;
-->
<!-- Document Head - Contains all NCX metadata.
-->
<!--ELEMENT head (smilCustomTest | meta)+-->
<!-- 2004-04-05 TM - only id and defaultState are copied -->
<!-- 2004-05-14 TM - revert to override copied too; added bookStruct attribute -->
<!-- smilCustomTest - Duplicates customTest data found in SMIL files.  Each unique customTest
element that appears in one or more SMIL files must have its id, defaultState and override
attributes duplicated in a smilCustomTest element in the NCX.  The NCX thus gathers in one
place all customTest elements used in the SMIL files, for presentation to the user.
-->
<!--ELEMENT smilCustomTest EMPTY-->
<!--ATTLIST smilCustomTest
id    ID    #REQUIRED
defaultState  (true|false)   'false'
override  (visible|hidden) 'hidden'
bookStruct  (PAGE_NUMBER|NOTE|NOTE_REFERENCE|ANNOTATION|LINE_NUMBER|OPTIONAL_SIDEBAR|OPTIONAL_PRODUCER_NOTE)  #IMPLIED
-->
<!-- Meta Element - metadata about this NCX -->
<!--ELEMENT meta EMPTY-->
<!--ATTLIST meta
  name    CDATA    #REQUIRED
  content  CDATA    #REQUIRED
  scheme  CDATA    #IMPLIED
-->
<!-- DocTitle - the title of the document, required and must immediately follow head.
-->
<!-- Revised, 3/31/2004:  Added img to content model -->
<!--ELEMENT docTitle (text, audio?, img?)-->
<!--ATTLIST docTitle
  id    ID    #IMPLIED
  %i18n;
-->
<!-- DocAuthor - the author of the document, immediately follows docTitle.
-->
<!-- Revised, 3/31/2004:  Added img to content model -->
<!--ELEMENT docAuthor (text, audio?, img?)-->
<!--ATTLIST docAuthor
  id    ID    #IMPLIED
  %i18n;
-->
<!-- Navigation Structure - container for all of the NCX objects that are part of the
hierarchical structure of the document.
-->
<!-- Revised, 3/31/2004:  Added navInfo to content model -->
<!--ELEMENT navMap (navInfo*, navLabel*, navPoint+)-->
<!--ATTLIST navMap
  id    ID    #IMPLIED
-->
<!-- Navigation Point - contains description(s) of target, as well as a pointer to
entire content of target.
Hierarchy is represented by nesting navPoints.  "class" attribute describes the kind
of structural unit this object represents (e.g., "chapter", "section").
-->
<!--ELEMENT navPoint (navLabel+, content, navPoint*)-->
<!-- Revised, 3/29/2004:  Removed onFocus/onBlur -->
<!-- Revised, 3/29/2004:  Removed value -->
<!-- Revised, 3/31/2004:  Removed pageRef -->
<!-- Revised, 4/5/2004:  Added playOrder -->
<!--ATTLIST navPoint
  id    ID      #REQUIRED
  class    CDATA    #IMPLIED
  playOrder CDATA       #REQUIRED
-->
<!-- Revised, 3/31/2004:  Added pageList element -->
<!-- Page List -  Container for pagination information.
  -->
<!--ELEMENT pageList (navInfo*, navLabel*, pageTarget+)-->
<!-- Revised, 4/5/2004:  Added class attribute -->
<!--ATTLIST pageList
   id       ID          #IMPLIED
   class    CDATA       #IMPLIED
-->
<!-- Revised, 3/31/2004:  Added pageTarget element -->
<!-- Revised, 4/5/2004:  Added description of value attribute to comment -->
<!-- Page Target -  Container for
  text, audio, image, and content elements containing navigational
  information for pages.  The "value" attribute is a positive integer representing
the numeric value associated with a page. Combination of values of type and
value attributes must be unique, when value attribute is present.
-->
<!--ELEMENT pageTarget (navLabel+, content)-->
<!-- Revised, 4/5/2004:  Added class attribute -->
<!-- Revised, 4/5/2004:  Changed declaration of type attribute to enumerate values -->
<!-- Revised, 4/5/2004:  Added playOrder -->
<!--ATTLIST pageTarget
   id       ID          #IMPLIED
   value    CDATA       #IMPLIED
   type     (front | normal | special)       #REQUIRED
   class    CDATA       #IMPLIED
   playOrder CDATA      #REQUIRED
-->
<!-- Navigation List - container for distinct, flat sets of navigable elements, e.g.
notes, figures, tables, etc.  Essentially a flat version of navMap.  The "class" attribute
describes the type of object contained in this navList, using dtbook element names, e.g., note.
-->
<!-- Revised, 3/31/2004:  Added navInfo to content model -->
<!--ELEMENT navList   (navInfo*, navLabel+, navTarget+) -->
<!--ATTLIST navList
  id    ID    #IMPLIED
  class    CDATA    #IMPLIED
-->
<!-- Revised, 4/5/2004:  Added description of value attribute to comment -->
<!-- Navigation Target - contains description(s) of target, as well as a pointer to
entire content of target.
navTargets are the equivalent of navPoints for use in navLists.  "class" attribute
describes the kind of structure this target represents, using its dtbook element
name, e.g., note.  The "value" attribute is a positive integer representing the
numeric value associated with the navTarget.
-->
<!--ELEMENT navTarget  (navLabel+, content) -->
<!-- Revised, 3/29/2004:  Removed onFocus/onBlur -->
<!-- Revised, 3/31/2004:  Removed mapRef -->
<!-- Revised, 4/5/2004:  Added playOrder -->
<!--ATTLIST navTarget
  id    ID    #REQUIRED
  class    CDATA    #IMPLIED
  value    CDATA    #IMPLIED
  playOrder CDATA       #REQUIRED
-->
<!-- Revised, 3/31/2004:  Added navInfo element -->
<!-- Navigation Information - Contains an informative comment
  about a navMap, pageList, or navList in various media for presentation to the user.
-->
<!--ELEMENT navInfo (((text, audio?) | audio), img?)-->
<!--ATTLIST navInfo
  %i18n;
-->
<!-- Navigation Label - Contains a description of a given <navMap>, <navPoint>,
<navList>, or <navTarget> in various media for presentation to the user. Can be
repeated so descriptions can be provided in multiple languages. -->
<!--ELEMENT navLabel (((text, audio?) | audio), img?)-->
<!--ATTLIST navLabel
  %i18n;
-->
<!-- Content Element - pointer into SMIL to beginning of navPoint. -->
<!--ELEMENT content EMPTY-->
<!--ATTLIST content
  id    ID    #IMPLIED
  src    %URI;    #REQUIRED
-->
<!-- Text Element - Contains text of docTitle, navPoint heading, navTarget (e.g., page number),
or label for navMap or navList. -->
<!--ELEMENT text (#PCDATA)-->
<!--ATTLIST text
  id    ID        #IMPLIED
  class  CDATA      #IMPLIED
-->
<!-- Audio Element - audio clip of navPoint heading. -->
<!--ELEMENT audio EMPTY-->
<!-- Revised, 3/29/2004:  clipBegin/clipEnd now REQUIRED -->
<!--ATTLIST audio
  id    ID        #IMPLIED
  class  CDATA      #IMPLIED
  src    %URI;      #REQUIRED
  clipBegin %SMILtimeVal;  #REQUIRED
  clipEnd  %SMILtimeVal;  #REQUIRED
-->
<!-- Image Element - image that may accompany heading. -->
<!--ELEMENT img EMPTY-->
<!--ATTLIST img
  id    ID      #IMPLIED
  class  CDATA    #IMPLIED
  src    %URI;    #REQUIRED
-->)xml";

constexpr std::string_view package = R"xml(

   default namespace = "http://www.idpf.org/2007/opf"
   namespace opf = "http://www.idpf.org/2007/opf"
   namespace dc = "http://purl.org/dc/elements/1.1/"

   include "./mod/datatypes.rnc"
   include "./mod/epub-prefix-attr.rnc"


   start = element package { opf.package.attlist & opf.package.content }

   opf.package.attlist &= opf.version.attr & opf.unique.identifier.attr & opf.id.attr? & epub.prefix.attr? & opf.i18n.attrs
   opf.version.attr = attribute version { '3.0' }
   opf.unique.identifier.attr = attribute unique-identifier { datatype.IDREF }

   opf.package.content = opf.metadata, opf.manifest, opf.spine, opf.guide?, opf.bindings?, opf.collection*

   opf.metadata = element metadata { opf.id.attr? & opf.i18n.attrs & opf.metadata.content }
   opf.metadata.content = opf.dc.elems & opf.meta* & opf.link*

   opf.meta = element meta {
      opf.epub3.meta.content | opf.epub2.meta.content
   }

   opf.epub3.meta.content = (opf.property.attr & opf.refines.attr? & opf.id.attr? & opf.scheme.attr? & opf.i18n.attrs & datatype.string.nonempty)
   opf.epub2.meta.content = (attribute name { text } & attribute content { text })  #legacy

   opf.link = element link { opf.href.attr & opf.rel.attr & opf.id.attr? & opf.refines.attr? & opf.media-type.attr? & opf.properties.attr? }

   opf.property.attr = attribute property { datatype.property }
   opf.rel.attr = attribute rel { datatype.properties }
   opf.scheme.attr = attribute scheme { datatype.property }
   opf.refines.attr = attribute refines { datatype.URI }

   opf.dc.identifier = element dc:identifier { opf.id.attr? & datatype.string.nonempty }
   opf.dc.title = element dc:title { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.language = element dc:language { opf.id.attr? & datatype.languagecode }
   opf.dc.date = element dc:date { opf.id.attr? & datatype.string.nonempty }
   opf.dc.source = element dc:source { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.type = element dc:type { opf.id.attr? & datatype.string.nonempty }
   opf.dc.format = element dc:format { opf.id.attr? & datatype.string.nonempty }
   opf.dc.creator = element dc:creator { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.subject = element dc:subject { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.description = element dc:description { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.publisher = element dc:publisher { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.contributor = element dc:contributor { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.relation = element dc:relation { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.coverage = element dc:coverage { opf.dc.attlist & datatype.string.nonempty }
   opf.dc.rights = element dc:rights { opf.dc.attlist & datatype.string.nonempty }

   opf.dc.elems = opf.dc.identifier+
                & opf.dc.title+
                & opf.dc.language+
                & opf.dc.date?
                & opf.dc.source*
                & opf.dc.type*
                & opf.dc.format*
                & opf.dc.creator*
                & opf.dc.subject*
                & opf.dc.description*
                & opf.dc.publisher*
                & opf.dc.contributor*
                & opf.dc.relation*
                & opf.dc.coverage*
                & opf.dc.rights*

   opf.dc.attlist = opf.id.attr? & opf.i18n.attrs

   opf.manifest = element manifest { opf.manifest.attlist & opf.manifest.content }
   opf.manifest.attlist &= opf.id.attr?
   opf.manifest.content = opf.item+


   opf.item = element item { opf.item.attlist }
   opf.item.attlist &= opf.id.attr & opf.href.attr & opf.media-type.attr & opf.fallback.attr? & opf.media-overlay.attr? & opf.properties.attr?

   opf.fallback.attr = attribute fallback { datatype.IDREF }
   opf.media-overlay.attr = attribute media-overlay { datatype.IDREF }
   opf.properties.attr = attribute properties { datatype.properties }


   opf.spine = element spine { opf.spine.attlist & opf.spine.content }
   opf.spine.attlist &= opf.id.attr? & opf.spine.toc.attr? & opf.ppd.attr?
   opf.spine.toc.attr = attribute toc { datatype.IDREF }
   opf.ppd.attr = attribute page-progression-direction { 'ltr' | 'rtl' | 'default'}
   opf.spine.content = opf.itemref+


   opf.itemref = element itemref { opf.itemref.attlist }
   opf.itemref.attlist &= opf.itemref.idref.attr & opf.itemref.linear.attr? & opf.id.attr? & opf.properties.attr?
   opf.itemref.idref.attr = attribute idref { datatype.IDREF }
   opf.itemref.linear.attr = attribute linear { 'yes' | 'no' }


   opf.guide = element guide { opf.reference+ }
   opf.reference = element reference { opf.href.attr & attribute type { datatype.text } & attribute title { datatype.text }?}

   opf.bindings = element bindings { opf.bindings.mediaType+ }
   opf.bindings.mediaType = element mediaType { opf.bindings.mediaType.attlist }
   opf.bindings.mediaType.attlist &= opf.media-type.attr & attribute handler { datatype.IDREF }

   opf.collection = element collection { opf.collection.attlist, opf.collection.content }
   opf.collection.attlist &= opf.id.attr? & opf.dir.attr? & opf.xml.lang.attr? & opf.collection.role.attr
   opf.collection.role.attr = attribute role { datatype.space.separated.tokens }
   opf.collection.content = opf.collection.metadata?, (opf.collection+ | (opf.collection*, opf.collection.link+))
   opf.collection.metadata = element metadata { opf.id.attr? & opf.i18n.attrs & opf.collection.metadata.content }
   opf.collection.metadata.content = opf.collection.dc.elems & opf.collection.meta* & opf.link*
   opf.collection.dc.elems = opf.dc.identifier*
                           & opf.dc.title*
                           & opf.dc.language*
                           & opf.dc.date*
                           & opf.dc.source*
                           & opf.dc.type*
                           & opf.dc.format*
                           & opf.dc.creator*
                           & opf.dc.subject*
                           & opf.dc.description*
                           & opf.dc.publisher*
                           & opf.dc.contributor*
                           & opf.dc.relation*
                           & opf.dc.coverage*
                           & opf.dc.rights*
   opf.collection.meta = element meta { opf.epub3.meta.content }
   opf.collection.link = element link { opf.href.attr & opf.rel.attr? & opf.id.attr? & opf.media-type.attr? }

   opf.media-type.attr = attribute media-type { datatype.mimetype }
   opf.href.attr = attribute href { datatype.URI }
   opf.id.attr = attribute id { datatype.ID }
   opf.i18n.attrs = opf.xml.lang.attr? & opf.dir.attr?
   opf.xml.lang.attr = attribute xml:lang { "" | datatype.languagecode }
   opf.dir.attr = attribute dir { 'ltr' | 'rtl' }
)xml";

    constexpr std::string_view package_nvdl = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<rules xmlns="http://purl.oclc.org/dsdl/nvdl/ns/structure/1.0" startMode="package">
    <mode name="package">
        <namespace ns="http://www.idpf.org/2007/opf">
            <validate schema="package-30.rnc" schemaType="application/relax-ng-compact-syntax"  useMode="attach"/>
            <validate schema="package-30.sch" useMode="attach"/>
        </namespace>
    </mode>
    <mode name="attach">
        <anyNamespace>
            <attach/>
        </anyNamespace>
    </mode>
</rules>)xml";

} // namespace

namespace DTD
{

    xmlpp::ustring get_ncx()
    {
        return xmlpp::ustring(NCX.begin(), NCX.end());
    }

    xmlpp::RelaxNGSchema get_package()
    {
      auto ptr = xmlRelaxNGNewMemParserCtxt(package_nvdl.data(), package_nvdl.size());
      auto parse = xmlRelaxNGParse(ptr);
      return xmlpp::RelaxNGSchema(parse);
    }
};
