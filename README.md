This 2-hour workshop will introduce Islandora objects and how they work within Drupal. It will also cover the basics of Islandora modules, and introduce Drupal hooks. The workshop doesn't cover Solr indexing, Drupal theming, writing tests for Islandora modules or access control in Islandora.

In the second hour of the workshop, participants will start coding their own simple Islandora module. To prepare for this part of the workshop, make sure you have installed the [Islandora Vagrant](https://github.com/Islandora-Labs/islandora_vagrant) virtual machine on your laptop.

## Outline

* Islandora objects
* Islandora's relationship with Drupal
* Tuque
* Islandora modules
  * Types: solution packs, integration modules utility modules
  * Structure
  * Islandora coding conventions
  * Islandora API
  * Hooks

## Islandora objects
Are fundamentally Fedora objects. Content models, structure/properties

## Islandora's relationship with Drupal


## Tuque


## Islandora modules

### Types

### Structure

Simplest:

```
.
├── islandora_foo.info
├── islandora_foo.module
├── LICENSE
├── README.md
```

Average complexity:
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

Hi complexity:

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


### Islandora API


### Hooks

A Drupal (and Islandora) hook is a special type of function that is fired at a predefined time in the execution of a page request. They are "defined" by a module and "implemented" by other modules. In this way, hooks lets a Drupal module add functionality that can be used by another module. Unlike a function, which you call to get an expected result from, a hook lets you write a function that always gets called by Drupal at a specific time.

For example, hook_islandora_object_access() is defined by the Islandora module, but is used by many other modules to determine whether a user is allowed to perform a specific action on an object.

Here is the entry in islandora.api.php for hook_islandora_object_access():

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
Hooks are fired in the providing module's code via the [Drupal API function](https://api.drupal.org/api/drupal/includes!module.inc/function/module_invoke_all/7) `[module_invoke_all()]`, which iterates through all modules that implement each hook and if it finds an implementation, fires it.

Islandora provides [many](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php) hooks. In this workshop, we'll focus on the ones that are fired during the object add/update/purge lifecycle, and use a couple of others.