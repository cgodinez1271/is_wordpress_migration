# Overview

This tool performs two functions; preparing WordPress blog exports in WXR for
migration, and integrating with the Drupal migrate module for actual migration.

## Pre-import

A number of transformations and preparations are prepared, including:

* Stripping comments (useful if using Disqus or a similar system)
* Combining separate files into one monolithic file
* Extracting author, tag, image information into separate files
* Replacing all WordPress shortcodes and filters with HTML

## Migration

The following migrations are included:

| Name of Migration | Mapping Review |
| --- | --- |
| EwWpAuthor | /admin/content/migrate/EwWpAuthor |
| EwWpImage | /admin/content/migrate/EwWpImage |
| EwWpRecapShow | /admin/content/migrate/EwWpRecapShow |
| EwWpRecapShowSeason | /admin/content/migrate/EwWpRecapShowSeason |
| EwWpPost | /admin/content/migrate/EwWpPost |
| EwWpCategory | /admin/content/migrate/EwWpCategory |
| EwWpTag | /admin/content/migrate/EwWpTag |
| EwWpTaxonomyRule | /admin/content/migrate/EwWpTaxonomyRule |

# Installation

Create the following folder:

````
sites/default/files/migration/wordpress/source
````

For each WordPress blog that you wish to process, add a sub-folder containing
all WXR (XML) files associated with the export. Unzipping the archive containing
the WXR files works quite nicely.

````
# Enable the module.
drush -y en ew_wordpress_migrate
# Clear caches.
drush cc all
drush cc drush
# Verify Migrations are ready.
drush migrate-status
# Import Taxonomy rules.
drush mi EwWpTaxonomyRule
````

# Tags and Categories

Since the target system has a number of custom taxonomies and vocabularies, a
straight 1:1 import isn't possible. Some terms will become content types, such
as a person or a creative work, and other terms will be mapped to a specific
vocabulary like event, genre, and so forth. Additionally, some terms are just
invalid and should be ignored instead of imported.

To handle all these transformations, a system has been devised that first
consolidates all the tags and categories in one place, then exports a
spreadsheet where the transformation logic can be defined. The resulting
spreadsheet can be edited by stakeholders with the appropriate domain knowledge
to make decisions about how the tags will be handled. When complete, the
spreadsheet is re-imported, customizing the rules. Once the rules are populated,
the actual migration can take place with the customized logic.

## Preparation

A number of steps are involved. First, pre-process all WordPress blogs using:

````
drush ew-wp-preimport
````

Then, run the following migrations to import all the tags.

````
drush mi EwWpCategory
drush mi EwWpTag
````

The tags themselves need customization by EW.com staff; to do that, it will need
to be exported:

````
# Writes to public://migration/wordpress/target/migration_taxonomy_rules.csv
drush ew-wp-tax-rule-export
# Selectively export non-customized rules; useful for deltas.
drush ew-wp-tax-rule-export --skip-existing
````

The spreadsheet is then customized, then placed in this module folder.

The rules can then be imported.

````
drush mi EwWpTaxonomyRule
````

Once the rules are in place, the WordPress content migrations can commence.

## The rule spreadsheet

The CSV file can be edited in any text or spreadsheet editor. Before import, it
must have Unicode (UTF-8) character encoding, Unix (LF) line endings, and have
a header row with the field names (it will be skipped on import).

| Field Name | Editable | Possible Values | Purpose |
| --- | --- | --- | --- |
| id | No | Integers | Primary key of Table |
| field_legacy_id | No | Integers | WordPress Term ID |
| source_term | No | String | The original term |
| blog_name | No | String | The source blog name, if any for context |
| source_type | No | tag, category | Context |
| rename_term | Yes | Any string | If set, will transform the term |
| ignore_term | Yes | 0, 1 | If set to 1, will not import the term |
| person | Yes | 0, 1 | If set to 1, will target Person content type, creating as necessary |
| movie | Yes | 0, 1 | If set to 1, will target Creative Work type Movie, creating as necessary |
| tv | Yes | 0, 1 | If set to 1, will target Creative Work type TV, creating as necessary |
| book | Yes | 0, 1 | If set to 1, will target Creative Work type Book, creating as necessary |
| music | Yes | 0, 1 | If set to 1, will target Creative Work type Music, creating as necessary |
| stage | Yes | 0, 1 | If set to 1, will target Creative Work type Stage, creating as necessary |
| web_series| Yes | 0, 1 | If set to 1, will target Creative Work type Web Series, creating as necessary |
| video_game| Yes | 0, 1 | If set to 1, will target Creative Work type Video Game, creating as necessary |
| taxonomy_genre| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |
| taxonomy_ew_franchise| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |
| taxonomy_franchise| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |
| taxonomy_event| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |
| taxonomy_freeform| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |
| taxonomy_category| Yes | 0, 1 | If set to 1, will add term to specified Taxonomy |

To consolidate terms, use the rename field. For example, with the following TV show:

* 666 Park 
* 666 Park Avenue

Specify the rename field for "666 Park" and set it to "666 Park Avenue".

For both entries, set tv to 1.

Make no change to "666 Park Avenue". The end result will be one Creative Work named "666 Park Avenue" type TV.

# Importing content from WordPress

Ensure that content types have all the necessary fields. Reverting features is
the easiest way.

````
# Revert all features.
drush fra -y
````

Prepare the files for import.

````
drush ew-wp-preimport --all_operations
# Specify folder to process.
````

Perform the import.

````
drush migrate-import EwWpAuthor
drush migrate-import EwWpImage
drush migrate-import EwWpRecapShow
drush migrate-import EwWpRecapShowSeason
drush migrate-import EwWpPost
# Undo all migrations.
drush migrate-rollback --all -y
````

## Testing images

If you don't feel like downloading every target image, there is a test mode
available.

Put a test image in ````public://test.png````, then edit image.inc and change
````$test_mode```` to ````TRUE;````. The migration will use the same paths and
file names, but the actual image will be the test image.

I recommend making the test image as small as possible.

# Known limitations

Does not account for or import:

* category
* wp:postmeta
    * blog-related-* - TBD
* pkg_label
* module_title - DEPRECATE
* module_classes - DEPRECATE
* module_more_link - DEPRECATE
* _publicize_pending - DEPRECATE
* tagazine-media - DEPRECATE
* sharing_disabled - DEPRECATE
* geo_public - DEPRECATE
* _ew_wp_meta
* image variations (not known if actual crops or just resized)

# Drush utilities

## ew-wp-preimport

Prepares WXR files for ingestion with the WordPress Migrate module. Recommend
using the ````--all_operations```` option for convenience.

Arguments:
````
 source                                    Folder within sites/default/files/migration/wordpress/source
````

Options:
````
 --all_operations                          If set, will perform every operation, such as removing comments, combining, etc.
 --combine                                 If set, creates one monolithic WXR file.
 --create_authors                          If set, will create an additional file, authors.xml, containing all authors.
 --create_tags                             If set, will create an additional file, tags.xml, containing all tags & categories.
 --ew_image_extract                        If set, creates ew_image.json containing all metadata ew_image tags.
 --img_src_extract                         If set, creates images.json containing all img tags and metadata.
 --remove_authors                          If set, will remove authors from each WXR file.
 --remove_comments                         If set, will remove comments from each WXR file.
 --remove_tags                             If set, will remove tags & categories from each WXR file.
 --shortcodes                              If set, replaces shortcodes with HTML.
````

## ew-wp-tax-rule-export

Export Taxonomy Term Rules for Migration into CSV format.

Options:
````
Options:
 --skip-existing                           If set, skips rules that have already been defined.
````

# WordPress shortcodes and filters

This was an interesting problem; how to render the hundreds of standard and
custom filters and shortcodes. The solution was to include a bare-minimum subset
of WordPress function files (see ````contrib/wordpress/includes````) and some
placeholder functions that do nothing. Search for "Colossal WordPress hack" to
see what was done and how. If something is rendering strangely, look in that
area first.

# Uninstall

When complete, disable and uninstall the module.

````
drush pm-disable ew_wordpress_migrate
drush pm-uninstall ew_wordpress_migrate
````

# Debugging

You can format the output to make it human readable.

XML:

````
xmllint --format combined.xml --output combined-working.xml
````

JSON:

````
cat images.json | python -mjson.tool > images-working.json
````
