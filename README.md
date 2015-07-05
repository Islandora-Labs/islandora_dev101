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

[Islandora provides many](https://github.com/Islandora/islandora/blob/7.x/islandora.api.php). In this workshop, we'll focus on the ones that are fired during the object add/update/purge lifecycle, and use a couple of others.