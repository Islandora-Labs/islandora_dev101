This 2-hour workshop will introduce Islandora objects and how they work within Drupal. It will also cover the basics of Islandora modules, and introduce Drupal hooks. The workshop doesn't cover Solr indexing, Drupal theming, writing tests for Islandora modules or access control in Islandora. We'll be covering Islandora 7.x-1.x development (that is, Islandora using FedoraCommons 3.x), not Islandora 7.x-2.x (using FedoraCommons 4.x).

The intended audience for the workshop is people who have some expeience developing in PHP but not necessarily experience with Drupal. Experience with Git will be useful.

In the second hour of the workshop, participants will start coding their own simple Islandora module. To prepare for this part of the workshop, make sure you have installed the [Islandora Vagrant](https://github.com/Islandora-Labs/islandora_vagrant) virtual machine on your laptop.

## Outline

* Setting up a development environment
* Islandora objects
* Islandora's relationship with Drupal
* Tuque
* Islandora modules
  * Types
  * Structure
  * Islandora coding conventions
  * Islandora API
  * Hooks
* Hands-on exercise

## Setting up a development environment

### Islandora Vagrant

The [Islandora Vagrant](https://github.com/Islandora-Labs/islandora_vagrant) virtual machine offers a full Islandora environment that is both easy to use and customizable.

### Drush

[Drush](http://www.drush.org/en/master/), the Drupal shell, is an essential tool for Drupal developers and administrators. Some commands you will end up using while you develop for Islandora:

* `drush cc all`: clears all of Drupal's caches.
* `drush en module_name`: enables the module with 'module_name'. If the module is hosted on Drupal.org, will also download it and all module dependencies if necessary.
* `drush dis module_name`: disables the module with 'module_name'.

You run drush commands from anywhere within the Drupal installation directory. Drush is installed and ready to use on the Islandora Vagrant VM.

### Some additional helpful tools

* The [Devel](https://www.drupal.org/project/devel) contrib module (also installed on the Islandora Vagrant VM)
* The [Islandora Sample Content Generator](https://github.com/mjordan/islandora_scg) module

## Islandora objects

Are fundamentally [FedoraCommons objects](https://wiki.duraspace.org/display/FEDORA38/Fedora+Digital+Object+Model), [RELS-EXT, RELS-INT](https://wiki.duraspace.org/display/FEDORA38/Digital+Object+Relationships). Content models, [structure/properties](https://github.com/Islandora/islandora/wiki/Working-With-Fedora-Objects-Programmatically-Via-Tuque#working-with-existing-objects)

### Properties and datastreams

An Islandora object has properties (including id, label, createdDate, models) and datastreams, which in turn have their own properties (including label, id, checksum, content, mimetype, versionable). All Islandora object have two reserved datastreams, RELS-EXT and DC. The DC datastream contains an XML representation of the object's simple Dublin Core metadata. The RELS-EXT datastream contains an XML representation of the object's relationships with external entities and contains information on the object's content model(s), the Islandora collections it is a member of, and other information. Here is an example of a simple RELS-EXT XML:

```xml
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:fedora="info:fedora/fedora-system:def/relations-external#" xmlns:fedora-model="info:fedora/fedora-system:def/model#" xmlns:islandora="http://islandora.ca/ontology/relsext#">
  <rdf:Description rdf:about="info:fedora/islandora:73">
    <fedora-model:hasModel rdf:resource="info:fedora/islandora:sp_basic_image"></fedora-model:hasModel>
    <fedora:isMemberOfCollection rdf:resource="info:fedora/islandora:sp_basic_image_collection"></fedora:isMemberOfCollection>
  </rdf:Description>
</rdf:RDF>
```

In practice, most objects also have two other datastreams, the "OBJ" datastream, which contains the file that the user uploaded to Islandora when the object was initially created, and the MODS datastream, which contains a MODS XML file describing the object. [MODS](http://www.loc.gov/standards/mods/) is Islandora's default descriptive metadata schema, although others can be used.

An Islandora object's properties can be accessed like any other PHP object's properties. Within Islandora modules, you usually have access to either a full Islandora object, or its PID (persistent identifier). A common pattern for accessing the object is:

```php
// If you only have a PID, you must load the corresponding Islandora object.
$object = islandora_object_load($pid);
if (!$object) {
  // Logic for object load failure would go here.
  return;
}
  // Logic for object load success would continue through the rest your code.
 ```
 
 To access the object's datastreams, you can use this pattern:
 
 ```php
 $datastream = islandora_datastream_load($dsid, $object);
 ```
 or
 
 ```php
 foreach ($object as $datastream) {
  strtoupper($datastream->id);
  $datastream->label = "new label";
  $datastream_content = $datastream->getContent();
}
```
 
 ### Content models
 
 All Islandora objects have one or more content models (most often only 1; having more than one is an edge case). The content model is assigned to an object by the solution pack when it creates the object (via a web form or a batch ingest, for example). An object's content model tells Islandora which edit forms to use, which datastreams are required and optional, and which viewer to use when rendering the object.
 
 Solution packs define the datastreams that make up an object, and those datastreams' mime types, in a "ds_composite_model.xml" file. For example, the PDF Solution Pack's [composite model file](https://github.com/Islandora/islandora_solution_pack_pdf/blob/7.x/xml/islandora_pdf_ds_composite_model.xml) shows that the OBJ datastream's mime type is "application/pdf", and that the MODS datastream is optional for objects of this content model.

## Islandora's relationship with Drupal

Islandora uses Drupal as a web development framework and incorpoprates all of Drupal's major subsystems:

* The Drupal module framework
* The Drupal menu system
* The hook system and most other parts of the Drupal API
* The theme layer
* The database access API
* The batch and queue APIs
* User management and access control
* Various APIs offered by various contrib modules such as Views, Rules, Pathauto, Context, and others

In fact, the only part of Drupal that Islandora doesn't use is the node subsystem. Islandora replaces Drupal nodes with Islandora objects. There are several modules that integrate Islandnora objects with Drupal nodes (for instance, [Islandora Sync](https://github.com/islandora/islandora_sync) and [Islandora Entity Bridge](https://github.com/btmash/islandora_entity_bridge)), but by default, Islandora does not create Drupal nodes corresponding to Islandora objects.

## Tuque

[Tuque](https://github.com/Islandora/tuque) is the PHP API that Islandora uses to communicate with FedoraCommons. 

[Docs](https://github.com/Islandora/islandora/wiki/Working-With-Fedora-Objects-Programmatically-Via-Tuque)

## Islandora modules

### Types

* solution packs: modules that define content models, provide add/edit forms, and viewers for various types of Islandora objects
  * [Book Solution Pack](https://github.com/Islandora/islandora_solution_pack_book)
  * [Compound Object Solution Pack](https://github.com/Islandora/islandora_solution_pack_compound)
  * [PDF Solution Pack](https://github.com/Islandora/islandora_solution_pack_pdf)
* viewer modules: modules that wrap viewers for datastreams
  * [Islandora OpenSeadragon](https://github.com/Islandora/islandora_openseadragon)
  * [Islandora PDF.js](https://github.com/Islandora/islandora_pdfjs)
* integration modules: integrate Islandora objects with other components of the Islandora stack or with Drupal APIs
  * [Islandora Solr Metadata](https://github.com/Islandora/islandora_solr_metadata)
  * [Islandora XML Forms](https://github.com/Islandora/islandora_xml_forms)
  * [Islandora XACML Editor](https://github.com/Islandora/islandora_xacml_editor)
* utility modules: modules that perform a set of tasks related to Islanodra objects or the Islandora stack
  * [Islandora Checksum](https://github.com/Islandora/islandora_checksum)
  * [Islandora Batch](https://github.com/Islandora/islandora_batch)
  * [Islandora Pathauto](https://github.com/Islandora/islandora_pathauto)

### Structure

The standard approach to extending Drupal is through modules. Islandora modules follow the patterns established for Drupal modules. The most basic Islandora module contains two files, an .info file and a .module file, within a directory. Here are some examples of the structure of Islandora modules, in increasing order of complexity:

Simplest (the module that you'll be playing with during this workshop):

```
.
├── islandora_dev101.info
├── islandora_dev101.module
├── LICENSE
├── README.md
```

Average complexity ([Islandora FITS](https://github.com/Islandora/islandora_fits)):
```
.
├── build
│   └── Doxyfile
├── build.xml
├── css
│   └── islandora_fits.css
├── includes
│   ├── admin.form.inc
│   └── derivatives.inc
├── islandora_fits.info
├── islandora_fits.install
├── islandora_fits.module
├── LICENSE.txt
├── README.md
└── theme
    └── islandora-fits-metadata.tpl.php
```

Hi complexity ([Islandora Book Solution Pack](https://github.com/Islandora/islandora_solution_pack_book)):

```
.
├── build
│   └── Doxyfile
├── build.xml
├── css
│   └── islandora_book.css
├── data
│   ├── datastreams
│   │   ├── islandora_bookCModel_ds_composite_model.xml
│   │   └── islandora_book_collection_policy.xml
│   └── forms
│       └── book_form_mods.xml
├── images
│   └── folder.png
├── includes
│   ├── admin.form.inc
│   └── manage_book.inc
├── islandora_book.info
├── islandora_book.install
├── islandora_book.module
├── LICENSE.txt
├── README.md
├── tests
│   ├── fixtures
│   │   └── test.tiff
│   ├── islandora_book_derivatives_ingest_purge.test
│   ├── islandora_book_ingest_purge.test
│   └── scripts
│       ├── adore.sh
│       └── tesseract.sh
└── theme
    ├── islandora-book-book.tpl.php
    ├── islandora-book-page-img-print.tpl.php
    ├── islandora-book-page.tpl.php
    └── theme.inc
```

### Islandora coding conventions

[The official wiki page](https://github.com/Islandora/islandora/wiki/Coding-Standards)

`drush dcs`


### Islandora API

[islandora.api.php](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php)

### Hooks

A Drupal (and Islandora) hook is a special type of function that is fired at a predefined time in the execution of a page request. They are "defined" by a module and "implemented" by other modules. In this way, hooks lets a Drupal module add functionality that can be used by another module. Unlike a function, which you call to get an expected return value, a hook lets you write a function that always gets called by Drupal at a specific time. More information on hooks is available from [drupal.org](https://www.drupal.org/node/292).

For example, hook_islandora_object_access() is defined by the Islandora module, but is used by many other modules to determine whether a user is allowed to perform a specific action on an object.

Here is the entry in [islandora.api.php](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php) for hook_islandora_object_access():

```php
/**
 * Object-level access callback hook.
 *
 * @param string $op
 *   A string define an operation to check. Should be defined via
 *   hook_permission().
 * @param AbstractObject $object
 *   An object to check the operation on.
 * @param object $user
 *   A loaded user object, as the global $user variable might contain.
 *
 * @return bool|NULL|array
 *   Either boolean TRUE or FALSE to explicitly allow or deny the operation on
 *   the given object, or NULL to indicate that we are making no assertion
 *   about the outcome. Can also be an array containing multiple
 *   TRUE/FALSE/NULLs, due to how hooks work.
 */
function hook_islandora_object_access($op, $object, $user) {
  switch ($op) {
    case 'create stuff':
      return TRUE;
    case 'break stuff':
      return FALSE;
    case 'do a barrel roll!':
      return NULL;
  }
}
```

The entry provides the function signature and the parameter/return documentationH, plus (in some caes) some sample code. The function signature begins with the word 'hook' but when we implement the hook in our modules, we replace that with the name of the module. This "magic" naming convention is how Drupal knows to fire the hook implementations.

Here are three implementations of the hook:

from islandora.module itself:

```php
/**
 * Implements hook_islandora_object_access().
 *
 * Denies according to PID namespace restrictions, then passes or denies
 * according to core Drupal permissions according to user_access().
 */
function islandora_islandora_object_access($op, $object, $user) {
  module_load_include('inc', 'islandora', 'includes/utilities');

  return islandora_namespace_accessible($object->id) && user_access($op, $user);
}
```

from islandora_scholar_embargo.module:

```php
/**
 * Implements hook_islandora_object_access().
 */
function islandora_scholar_embargo_islandora_object_access($op, $object, $user) {
  module_load_include('inc', 'islandora_scholar_embargo', 'includes/embargo');
  if ($op == 'administer islandora_xacml_editor') {
    $embargoed = islandora_scholar_embargo_get_embargoed($object);
    if (!empty($embargoed)) {
      return FALSE;
    }
  }
}
```
from islandora_basic_collection.module:

```php
/**
 * Implements hook_islandora_object_access().
 *
 * Maps our three permissions onto those in the Islandora core.
 */
function islandora_basic_collection_islandora_object_access($op, $object, $user) {
  $result = NULL;

  $collection_models = islandora_basic_collection_get_collection_content_models();
  $is_a_collection = (
    (count(array_intersect($collection_models, $object->models)) > 0) &&
    isset($object['COLLECTION_POLICY'])
  );

  if (in_array($op, array_keys(islandora_basic_collection_permission()))) {
    if ($is_a_collection) {
      if ($op == ISLANDORA_BASIC_COLLECTION_CREATE_CHILD_COLLECTION) {
        $result = islandora_object_access(ISLANDORA_INGEST, $object, $user) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object['COLLECTION_POLICY'], $user);
        if ($result) {
          $policy = new CollectionPolicy($object['COLLECTION_POLICY']->content);
          $policy_content_models = $policy->getContentModels();
          $result = count(array_intersect($collection_models, array_keys($policy_content_models))) > 0;
        }
      }
      elseif ($op == ISLANDORA_BASIC_COLLECTION_MANAGE_COLLECTION_POLICY) {
        $result = islandora_datastream_access(ISLANDORA_METADATA_EDIT, $object['COLLECTION_POLICY'], $user);
      }
      elseif ($op == ISLANDORA_BASIC_COLLECTION_MIGRATE_COLLECTION_MEMBERS) {
        // Not sure how much sense this makes... Check that we can modify the
        // RELS-EXT of the current object, assuming that we'll be able to modify
        // the children as well...
        $result = islandora_datastream_access(ISLANDORA_METADATA_EDIT, $object['RELS-EXT'], $user);
      }
    }
    else {
      $result = FALSE;
    }
  }

  return $result;
}
```

Hooks are fired in the providing module's code via the [Drupal API function](https://api.drupal.org/api/drupal/includes!module.inc/function/module_invoke_all/7) `module_invoke_all()`, which iterates through all modules that implement each hook and if it finds an implementation, fires it.

Islandora provides [many](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php) hooks. In the first exercise below, we'll implement the ones that are fired during the object add/update/purge lifecycle, and a couple of others.

## Hands-on exercise 1: Displaying messages

In this exercise, we will implement a few Islandora hooks that will let us display a message to the user when objects are added, modified, and deleted.

## Hands-on exercise 2: Exploring islandora.api.php

In this exercise, we will take a detailed look at islandora.api.php, the standard documentation for the hooks that Islandora provides to developers. We will also look at some examples in Islandora modules.

## Hands-on exercise 3: Do something with a datastream

In this exercise, we will implement `hook_islandora_datastream_modified()` to display a message to the user when she has modified the MODS datastream.
