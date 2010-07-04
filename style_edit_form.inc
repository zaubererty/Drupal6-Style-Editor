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

function style_edit_stylesheet_edit_form($formstate,$cssid) {

    $styles = style_edit_load_stylesheet_db($cssid);

    $form = array();
    $form['css'] = array();
    $form['css']['#tree'] = TRUE;

    foreach ($styles as $style) {

        if (count($style) < 2 ) continue;

        $from['css'][$style['styleid']] = array(
                'selector' => array(
                        '#type' => 'markup',
                        '#value' => $style['selector'],
                ),
                'style' => array(
                        '#type' => 'markup',
                        '#value' => $style['style'],
                ),
                'edit_op' => array(
                        '#type' => 'markup',
                        '#value' => l(t(edit), 'admin/settings/styleedit/editstyle/'.$style['styleid']),
                ),
                'weight' => array(
                        '#type' => 'weight',
                        '#delta' => $styles['count'],
                        '#default_value' => $style['weight'],
                ),
                'styleid' => array(
                        '#type' => 'hidden',
                        '#value' => $style['styleid'],
                ),
        );
    }
    
    $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),

    );

    return $from;
}

?>
