<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
*/

function style_edit_styleset_edit_form($formstate,$styleid) {
    $data = style_edit_load_stylesection_db($styleid);

    $form = array();
    
    $from['sel_cat'] = array(
            '#type' => 'fieldset',
            '#title' => t('Selector and category'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $from['sel_cat']['selector'] = array(
            '#type' => 'textfield',
            '#title' => t('Selector'),
            '#default_value' => $data[1]['selector'],
    );

    $from['style'] = array(
            '#type' => 'fieldset',
            '#title' => t('Styles'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $from['style']['css'] = array(
            '#type' => 'textarea',
            '#title' => t('CSS'),
            '#rows' => 10,
            '#default_value' => clear_brackets($data[1]['style']),
    );

    $from['styleid'] = array(
            '#type' => 'value',
            '#value' => $data[1]['styleid'],
    );
    
    $from['submit'] = array(
            '#type' => 'submit',
            '#value' => t('save'),
    );

    return $from;
}

function style_edit_styleset_edit_form_submit($form, &$form_state) {
    //Update the database with the new values
    //dargs();
    $val = $form_state['values'];
    db_query("UPDATE {style_edit_cssstyles} SET selector = '%s', style = '%s' WHERE styleid = %d LIMIT 1",
            array($val['sel_cat']['selector'],'{'.$val['style']['css'].'}' ,$val['styleid']));
    drupal_set_message(t('Saved style ID '. $val['styleid']));
}

function style_edit_stylesheet_edit_form($formstate, $cssid = 1) {
    $styles = style_edit_get_stylesheet_db($cssid);

    $form = array();

    $form['styleitems'] = array();
    $form['styleitems']['#tree'] = TRUE;
    //$form['styleitems']['#theme'] = 'style_edit_stylesheet_edit_form';

    $xx = 0;
    foreach ($styles as $key => $style) {

        if (count($style) < 2) continue;

        if ($xx > 10) continue;
        $xx++;

        $sel = $style['selector'];
        $css = escape_html($style['style']);
        $sort = $style['weight'];
        $sid = $style['styleid'];

        $form['styleitems'][$key]['selector'] = array(
            '#value' => $sel,
        );
        $form['styleitems'][$key]['style'] = array(
            '#value' => $css,
        );
        $form['styleitems'][$key]['edit_op'] = array(
            '#value' => $sid,
        );
        $form['styleitems'][$key]['sort'] = array(
            '#type' => 'weight',
            '#delta' => count($styles),
            '#default_value' => $sort,
        );
        $form['styleitems'][$key]['sid'] = array(
            '#input' => 'hidden',
            '#value' => $sid,
        );
    }

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#prefix' => '<br><div class="poll-form">',
        '#suffix' => '</div>',
    );


    //dpm($form['submit']);
    return $form;
}

function style_edit_stylesheet_edit_form_submit($form, &$form_state) {
    //Update the database with the new values'
    dargs();
    foreach ($form_state['values']['css'] as $item) {
        db_query('UPDATE {style_edit_cssstyles} SET weight = %d WHERE styleid = %d LIMIT 1',
                array($item['weight'], $item['id']));
        drupal_set_message('Saved ID'.$item['id']);
    }
}

function theme_style_edit_stylesheet_edit_form($form){
    drupal_add_tabledrag('css_table', 'order', 'sibling', 'sort');

    $header = array('',t('Selector'),t('Style'),t('Operation'),'sort');
    foreach (element_children($form['styleitems']) as $key) {

        $element = &$form['styleitems'][$key];
        $element['sort']['#attributes']['class'] = 'sort';
        $element['itemid']['#attributes']['class'] = 'itemid';

        $row = array('');
        $row[] = drupal_render($element['selector']);
        $row[] = drupal_render($element['style']);
        $row[] = drupal_render($element['edit_op']);
        $row[] = drupal_render($element['sort']).
                    drupal_render($element['sid']);

        //Add a draggable class to every table row (<tr>)
        $rows[] = array('data' => $row, 'class' => 'draggable');
    }

    $output = theme('table', $header, $rows, array('id' => 'css_table'));

    $output .= drupal_render($from);
    return $output;
}


/**
 *  Load the Stylesheet from the database
 *
 * @param   int     $cssid  ID of the stylesheed to load
 * @return  array           List of items and count field
 */

function style_edit_get_stylesheet_db($cssid) {
    $sql = 'SELECT * FROM {style_edit_cssstyles} WHERE cssid = %d ORDER BY weight';
    $result = db_query($sql,$cssid);
    $table[] = array();
    while ($r = db_fetch_array($result)) {
        $table[] = $r;
    }
    //$table['count'] = $result->num_rows;
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

/**
 *makes html visible
 *
 * @param   string  $text   Text to clean
 * @return  string          Clean text
 */
function escape_html($text) {
    $text = str_replace('<', '&lt;', $text);
    $text = str_replace('>', '&gt;', $text);
    return $text;
}

?>
