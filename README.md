## Overview

This 2-hour workshop will introduce Islandora objects and how they work within Drupal. It will also cover the basics of Islandora modules, and introduce Drupal/Islandora hooks. The workshop does not cover Solr indexing, Drupal theming, writing tests for Islandora modules or access control in Islandora. We'll be covering Islandora 7.x-1.x development (that is, Islandora using FedoraCommons 3.x), not Islandora 7.x-2.x (using FedoraCommons 4.x).

The intended audience for the workshop is people who have some expeience developing in PHP but not necessarily experience with Drupal. Experience using text editors such as vim, nano/pico or Emacs will be very useful, as will experience using Git.

In the second hour of the workshop, participants will start coding their own simple Islandora module. To prepare for this part of the workshop, make sure you have the following installed on your laptop:

* the [Islandora Vagrant](https://github.com/Islandora-Labs/islandora_vagrant) virtual machine
* an ssh client
* an scp client if you prefer to work in a graphical text editor on your laptop

The workshop will cover the following topics:

* Setting up a development environment
* Islandora objects
* Islandora's relationship with Drupal
* Islandora other major components
* Islandora modules
  * Types
  * Structure
  * Islandora coding conventions
  * Islandora API
  * Hooks
* Hands-on exercises

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

Islandora objects are the basic structural component of content within an Islandora repository. Objects contain datastreams, and are containied within parent objects, typically Islandora collections but in some cases as children of compound objects. Islandora objects are [FedoraCommons objects](https://wiki.duraspace.org/display/FEDORA38/Fedora+Digital+Object+Model) that follow specific conventions surrounding properties, datastreams, and content models.

### Properties and datastreams

An Islandora object's content is stored in datastreams. The simplest way to imagine the relationship between objects and datastreams is that if an object is an email messages, datastreams are its attachments. The email has structure, but apart from the small amount of content within the message, there may be images, movies, and PDFs are associated with the message and linked to it.

An Islandora object has properties (including id, label, createdDate, models) and datastreams, which in turn have their own properties (including label, id, checksum, content, mimetype, versionable). Full lists of [object properties](https://github.com/Islandora/islandora/wiki/Working-With-Fedora-Objects-Programmatically-Via-Tuque#properties) and [datastream properties](https://github.com/Islandora/islandora/wiki/Working-With-Fedora-Objects-Programmatically-Via-Tuque#properties-1) are available. All Islandora objects have two reserved datastreams, RELS-EXT and DC. The DC datastream contains an XML representation of the object's simple Dublin Core metadata. The RELS-EXT datastream contains an XML representation of the object's relationships with external entities and contains information on the object's content model(s), the Islandora collections it is a member of, and other information. Here is an example of a simple RELS-EXT XML:

```xml
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:fedora="info:fedora/fedora-system:def/relations-external#" xmlns:fedora-model="info:fedora/fedora-system:def/model#" xmlns:islandora="http://islandora.ca/ontology/relsext#">
  <rdf:Description rdf:about="info:fedora/islandora:73">
    <fedora-model:hasModel rdf:resource="info:fedora/islandora:sp_basic_image"></fedora-model:hasModel>
    <fedora:isMemberOfCollection rdf:resource="info:fedora/islandora:sp_basic_image_collection"></fedora:isMemberOfCollection>
  </rdf:Description>
</rdf:RDF>
```

In practice, most Islandora objects also have two other datastreams, the OBJ datastream, which contains the file that the user uploaded to Islandora when the object was initially created, and the MODS datastream, which contains a MODS XML file describing the object. [MODS](http://www.loc.gov/standards/mods/) is Islandora's default descriptive metadata schema, although others can be used. 

An Islandora object's properties can be accessed like any other PHP object's properties. Within Islandora modules, you usually have access to either a full Islandora object, or its PID (persistent identifier). A common pattern for accessing the object is:

```php
// If you only have a PID, you must load the corresponding Islandora object.
$object = islandora_object_load($pid);
if ($object) {
  // You can get the object's properties.
  $my_pid = $object->id;
  $my_label = $object->label;
  // Or you can set its properties.
  $object->label = 'New label';
  $relationships = $object->relationships;
}
 ```
 
 To access the object's datastreams, you can use this pattern:
 
 ```php
 $datastream = islandora_datastream_load($dsid, $object);
 ```
 or
 
 ```php
 foreach ($object as $datastream) {
  // Datastream properties can be gotten and set in ways similar to objects' properties.
  $ds_label = $datastream->label;
  $datastream->label = "new label";
  $datastream_content = $datastream->content;
}
```

 
### Content models
 
All Islandora objects have one or more content models (most often only one; having more than one is an edge case). The content model is assigned to an object by the solution pack when it creates the object (via a web form or a batch ingest, for example). An object's content model tells Islandora which edit forms to use, which datastreams are required and optional, and which viewer to use when rendering the object. You can access an object's content models using the `$object->models` property.
 
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

In fact, the only part of Drupal that Islandora doesn't use is the [entity/node subsystem](https://www.drupal.org/node/1261744). Islandora replaces Drupal nodes with Islandora objects. There are several modules that integrate Islandnora objects with Drupal nodes (for instance, [Islandora Sync](https://github.com/islandora/islandora_sync) and [Islandora Entity Bridge](https://github.com/btmash/islandora_entity_bridge)), but by default, Islandora does not create Drupal nodes corresponding to Islandora objects.

## Islandora's other major components

In addition to Drupal, Islandora uses the following applications and libraries.

### Fedora Commons (a.k.a. Fedora Repository, a.k.a. FCREPO)
[Fedora Repository](https://wiki.duraspace.org/display/FF/Downloads) provides low-level asset management services within Islandora, including storage, access control, versioning, and checksumming. Islandora's object model, described above, is inherited directly from Fedora Repository's. An important part of Fedora Commons that is used heavily in Islandora is the [Resource Index](https://wiki.duraspace.org/display/FEDORA38/Resource+Index) (abbrieviated RI), which provides a query interface for basic object properties. The Resource Index is used heavily throughout Islandora.

As mentioned earlier, the current version of Islandora, 7.x-1.x, uses Fedora Repository 3.x, but the Islandora community [is migrating](https://github.com/Islandora/Islandora-Fedora4-Interest-Group) to Fedora Repository version 4.x. Islandora running Fedora 4.x will have the version number 7.x-2.x.

### Solr

[Apache Solr](http://lucene.apache.org/solr/) provides search and retrieval functionality for metadata contained in Islandora objects' XML datastreams, and for full text of books, newspapers, and other types of content. Solr extracts queriable content from datastreams using a third-party application called the [Generic Search Service](https://wiki.duraspace.org/display/FCSVCS/Generic+Search+Service+2.7) (or more commonly, gsearch).

The general difference between Fedora's Resource Index and Solr is that the RI is used for querying object properties, while Solr is used for querying datastream content. For example, when an Islandora module needs to find all of the pages in a book, it will query the RI; when a user wants to find all books that contain the keywords "dog" and "sled", Islandora queries Solr.

### Tuque

[Tuque](https://github.com/Islandora/tuque) is the PHP API that Islandora uses to communicate with FedoraCommons. Tuque is basically a wrapper around Fedora's REST interface. Tuque is `included` by the core Islandora module, which makes its functionality available within the Drupal environment for all other modules. Module developers don't need to `include` it in their code themselves.

Tuque is completely usable outside of the Islandora codebase as a standalone API library. Here is a sample script that connects to the Fedora repository and issues a query against the Resource Index to get a list of all objects that are children of a specific collection.

```php
<?php
/**
 * Sample tuque script that queries the Fedora Resource Index to get a list of
 * all objects that are children of the islandora:sp_basic_image_collection.
 */

$fedora_user = 'fedoraAdmin';
$fedora_pass = 'fedoraAdmin';
$fedora_url = 'http://localhost:8080/fedora';
$collection = 'islandora:sp_basic_image_collection';

/**
 * Load Tuque API.
 */
include_once 'tuque/Datastream.php';
include_once 'tuque/FedoraApi.php';
include_once 'tuque/FedoraApiSerializer.php';
include_once 'tuque/Object.php';
include_once 'tuque/RepositoryConnection.php';
include_once 'tuque/Cache.php';
include_once 'tuque/RepositoryException.php';
include_once 'tuque/Repository.php';
include_once 'tuque/FedoraRelationships.php';

// Connect to the Fedora repo.
try {
  $connection = new RepositoryConnection($fedora_url, $fedora_user, $fedora_pass);
}
catch (Exception $e) {
  print $e->getMessage();
}

$api = new FedoraApi($connection);
$cache = new SimpleCache();
$repository = new FedoraRepository($api, $cache);

// Query the rindex to get all the objects that have a 'isMemberOfCollection'
// relationship with the specified collection.
$ri_query = 'select $object from <#ri>
  where  $object <fedora-rels-ext:isMemberOfCollection> <info:fedora/' . $collection . '>';
  $members = $repository->ri->itqlQuery($ri_query, 'unlimited');

print "Members of the '$collection' collection\n\n";

// Printn out their PIDs.
foreach ($members as $member) {
  print $member['object']['value'] . "\n";
}
```

The standard [documentation](https://github.com/Islandora/islandora/wiki/Working-With-Fedora-Objects-Programmatically-Via-Tuque) for Tuque is essential reading and reference material for Islandora developers.

## Islandora modules

### Types

Islandora modules can be grouped into four rough categories:

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

These categories are for overview purposes only; there are no code-level or structural requirements for the various types. Two exceptions are that 1) solution packs generally provide colleciton policy and MODS forms definition files, and implement some specific functions that register these; and 2) viewer modules implement `hook_islandora_viewer_info()`.

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

Like any quality open source software application and community, Islandora uses a set of well-documented coding conventions. [The official wiki page](https://github.com/Islandora/islandora/wiki/Coding-Standards) provides explanations and examples. When you are coding Islandora modules, you should get in the habbit of running the following command within your module directory:

`drush dcs`

This command runs executes PHP_CodeSniffer, the standard PHP source code style analysis tool, to enforce Drupal coding standards, and if it finds any, will provide a detailed report.

The most common errors many developers make are:

* improper indentation (use 2 spaces for each level)
* white space at the end of lines (this is not allowed)
* missing file and function comments.

Many Islanodra developers configure their text editors and IDEs to use PHP_CodeSniffer.


### Islandora API

The core Islandora module provides an API that module developers can use. This API includes a number of Drupal hooks, documented at [islandora.api.php](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php), and a number of helper functions, many of which are defined in various .inc files within the [Islandora module's includes directory](https://github.com/Islandora/islandora/tree/7.x/includes). `islandora_object_load()`, which is defined in the islandora.module file, is an example of these helper functions.

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

Islandora provides many hooks. In the first exercise below, we'll implement the ones that are fired during the object add/update/purge lifecycle, and a couple of others.

## Hands-on exercices

These exercises focus on understanding and implementing Islandora hooks. Their usefulness as solutions to real problems is secondary to their ability to illustrate when hooks are fired, and what variables are available within the implemented hook functions.

When we write code in exercies 1 and 3, we'll use simple functions such as `drupal_set_message()` and `dd()` to "do something". The module file `islandora_dev101.module` provides some scaffolding for the exercises. Exercise 2 is not a coding exercise, but it will get us looking at the documentation for hooks and at examples implementations in other modules.

### Exercise 1: Detecting when objects are added, modified, or purged

In this exercise, we will implement a few Islandora hooks that will let us display a message to the user when objects are added, modified, and deleted.

1. Open the islandora_dev101.module file in an editor and add either `drupal_set_message()` or `dd()` statements where it says "// Do something" in the three function signatures (that is, hook implementations) at the bottom of the file. If you prefer to write non-Drupal PHP code (e.g., file_put_content(), etc.) instead of using Drupal functions, that's fine.
2. Your code should print a property of the Islandora object, such as ID or owner.
3. As a bonus, configure your own message in the module's admin settings and then retrieve that message within your functions using `variable_get('islandora_dev101_message', 'This just happened: ')`. Print the message along with the object property.

### Exercise 2: Exploring islandora.api.php

In this exercise, we will take a detailed look at [islandora.api.php](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php), the standard documentation for the hooks that Islandora provides to developers. We will also look at some examples in Islandora modules.

1. Look up the entry for `hook_islandora_object_alter()` in the API documentation
2. Find an implementation of this hook in an Islandora module.
3. Repeat for `hook_islandora_datastream_modified()`, `hook_islandora_ingest_steps()`, and `hook_islandora_derivative()`.

### Exercise 3: Doing something with a datastream

In this exercise, we will implement a hook that will write the content of an object's DC datastream to a file every time an object is viewed.

1. Find a suitable Islandora hook that will fire when an object is viewed. Your instructor will have some suggestions.
2. Open the islandora_dev101.module file in an editor and add your implementation of the hook.
3. Access the DC datastream's content using `$object['DC']->content` and print it to a file using code similar to what you wrote in exercise 1.
4. As a bonus, print the content of the RELS-EXT datastream.
