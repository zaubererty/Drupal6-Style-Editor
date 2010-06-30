<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
function style_edit_styleset_edit_form($styleset) {
    $from['sel_cat'] = array(
        '#type' => 'fieldset',
        '#title' => t('Selector and category'),
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        );

    $from['sel_cat']['selector'] = array(
        '#type' => 'textfield',
        '#title' => t('Selector'),
        );

    $from['size_pos'] = array(
        '#type' => 'fieldset',
        '#title' => t('Size and position'),
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        );

    $from['size_pos']['width'] = array(
        '#type' => 'textarea',
        '#title' => t('CSS'),
        '#value' => '',
        );

    dpm($from);
    return $from;
}

?>
