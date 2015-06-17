This 2-hour workshop will introduce Islandora objects and how they work within Drupal. It will also cover the basics of Islandora module development, and introduce Drupal hooks.

The workshop doesn't cover Drupal theming, writing tests for Islandora modules or access control in Islandora. In the second hour of the workshop, participants will start coding their own simple Islandora module.

## Outline

* Islandora objects
* Islandora's relationship with Drupal
* Tuque
* Islandora modules
  * Types: solution packs and utility modules
  * Structure
  * Islandora coding conventions
  * Islandora API
  * Hooks
* Participating in the Islandora development community

## Islandora objects
Content models, structure/properties

## Islandora's relationship with Drupal


## Tuque


## Islandora modules

### Types

### Structure

Simplest:

```
.
├── islandora_foo.info
├── islandora_fooodule
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


## Participating in the Islandora development community
