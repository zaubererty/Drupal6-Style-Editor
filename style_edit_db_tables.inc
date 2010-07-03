<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

function style_edit_schema_definition(){
    $schema = array_merge(
            style_edit_schema_cssimport(),
            style_edit_schema_cssstyles()
            );
    return $schema;
}

function style_edit_schema_cssimport() {
    $schema['style_edit_cssimport'] = array(
        'description' => t('Store the imported css file metadata'),
        'fields' => array(
            'cssid' => array(
                'type' => 'serial',
                'not null' => TRUE,
                'description' => t('Primary key'),
            ),
            'source' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
                'description' => t('Kind of source for the CSS file '),
            ),
            'name' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
                'description' => t('Display name of theme / module'),
            ),
            'systemname' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
                'description' => t('System theme / module name'),
            ),
            'filename' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
                'description' => t('just the CSS file name'),
            ),
            'path' => array(
                'type' => 'varchar',
                'length' => 512,
                'not null' => TRUE,
                'description' => t('Path to CSS file'),
            ),
            'ufiletimestamp' => array(
                'type' => 'int',
                'unsigned' => true,
                'description' => t('last imput of the given CSS file'),
            ),
        ),
        'primary key' => array(
            'cssid'
        ),
    );
    return $schema;
}

function style_edit_schema_cssstyles() {
    $schema['style_edit_cssstyles'] = array(
        'description' => t('Store the imported css file metadata'),
        'fields' => array(
            'styleid' => array(
                'type' => 'serial',
                'not null' => TRUE,
                'description' => t('Primary key'),
            ),
            'kind' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
                'description' => t('type of style entry'),
            ),
            'selector' => array(
                'type' => 'text',
                'not null' => TRUE,
                'description' => t('CSS selector'),
            ),
            'style' => array(
                'type' => 'text',
                'not null' => TRUE,
                'description' => t('CSS functiones'),
            ),
            'weigth' => array(
                'type' => 'int',
                'not null' => TRUE,
                'description' => t('Sorting info'),
            ),
            'cssid' => array(
                'type' => 'int',
                'not null' => TRUE,
                'description' => t('Parent key'),
            ),
        ),
        'primary key' => array(
            'styleid'
            ),
        'indexes' => array(
            'cssid' => array(
                'cssid'
                ),
         ),
    );
    return $schema;
}
?>
