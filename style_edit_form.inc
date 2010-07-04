<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
*/

function style_edit_styleset_edit_form($formstate,$styleid) {
    $data = style_edit_load_stylesection_db($styleid);

    $from['sel_cat'] = array(
            '#type' => 'fieldset',
            '#title' => t('Selector and category'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $from['sel_cat']['selector'] = array(
            '#type' => 'textfield',
            '#title' => t('Selector'),
            '#value' => $data[1]['selector'],
    );

    $from['css'] = array(
            '#type' => 'fieldset',
            '#title' => t('Size and position'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $from['css']['width'] = array(
            '#type' => 'textarea',
            '#title' => t('CSS'),
            '#value' => clear_brackets($data[1]['style']),
    );

    $from['styleid'] = array(
            '#type' => 'hidden',
            '#value' => $data[1]['styleid'],
    );

    $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
    );

    return $from;
}

function style_edit_stylesheet_edit_form_submit($form, &$form_state) {
    //Update the database with the new values
    $val = $form_state['values'];
    db_query("UPDATE {style_edit_cssstyles} SET selector = '%s', style = '%s' WHERE styleid = %d LIMIT 1",
            array($form_state['values'], $item['styleid']));
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
    dpm($from);
    return $from;
}

function style_edit_stylesheet_edit_form_submit($form, &$form_state) {
    //Update the database with the new values
    foreach ($form_state['values']['css'] as $item) {
        db_query('UPDATE {style_edit_cssstyles} SET weight = %d WHERE styleid = %d LIMIT 1',
                array($item['weight'], $item['styleid']));
    }
}

/**
 *  Load the Stylesheet from the database
 *
 * @param   int     $cssid  ID of the stylesheed to load
 * @return  array           List of items and count field
 */

function style_edit_load_stylesheet_db($cssid) {
    $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE cssid = %d ORDER BY weigth';
    $result = db_query($sql,$cssid);
    $table[] = array();
    while ($r = db_fetch_array($result)) {
        $table[] = $r;
    }
    $table['count'] = $result->num_rows;
    return $table;
}

/**
 *  Load the Stylesheet from the database
 *
 * @param   int     $styleid    ID of the stylesheed section to load
 * @return  array               List of items
 */

function style_edit_load_stylesection_db($styleid) {
    $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE styleid = %d ';
    $result = db_query($sql,$styleid);
    $table[] = array();
    while ($r = db_fetch_array($result)) {
        $table[] = $r;
    }
    return $table;
}


/**
 *Cleans {}
 *
 * @param   string  $text   Text to clean
 * @return  string          Clean text
 */
function clear_brackets($text) {
    $text = str_replace('{', '', $text);
    $text = str_replace('}', '', $text);
    return $text;
}

?>
