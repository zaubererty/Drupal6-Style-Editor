<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

function style_edit_schema(){
    $schema[] = style_edit_schema_cssimport();
    return $schema;
}

function style_edit_schema_cssimport() {
    $schema['style_edit_cssimport'] = array(
        'description' => t('Store the imported css file metadata'),
        'fields' => array(
            'cssid' => array(
                'type' => 'serial',
                'not null' => TRUE,

            ),
            'type' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,

            ),
            'name' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
            ),
            'systemname' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
            ),
            'filename' => array(
                'type' => 'varchar',
                'length' => 255,
                'not null' => TRUE,
            ),
            'path' => array(
                'type' => 'varchar',
                'length' => 512,
                'not null' => TRUE,
            ),
            'type' => array(
                'ufiletimestamp' => 'int',
            ),
        ),
        'primary key' => array(
            'cssid'
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
