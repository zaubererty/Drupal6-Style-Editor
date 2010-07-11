<?php
// $Id$
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
*/

function style_edit_styleset_edit_form($formstate, $theme, $styleid) {
    $data = get_stylesection_db($styleid);

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

    $from['sel_cat']['kind'] = array(
            '#type' => 'select',
            '#title' => t('Type'),
            '#default_value' => $data[1]['kind'],
            '#options' => array(
                'COMMENT' => t('Comment'),
                'STYLE' => t('Style'),
             ),
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
    
    $from['cssid'] = array(
            '#type' => 'value',
            '#value' => $data[1]['cssid'],
    );

    $from['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
    );

    return $from;
}

function style_edit_styleset_edit_form_submit($form, &$form_state) {
    //Update the database with the new values
    //dargs();
    $val = $form_state['values'];
    switch ($val['sel_cat']['kind']) {
        case 'STYLE':
            $insert_val = array($val['sel_cat']['selector'],'{'.$val['style']['css'].'}' ,$val['styleid']);
            break;
        case 'COMMENT':
            $insert_val = array($val['sel_cat']['selector'],$val['style']['css'] ,$val['styleid']);
            break;
        default:
            drupal_set_message(t('Type not known!'), 'error');
            break;
    }
    db_query("UPDATE {style_edit_cssstyles} SET selector = '%s', style = '%s' WHERE styleid = %d LIMIT 1",
            $insert_val);

    db_query('UPDATE {style_edit_cssimport} SET udbtimestamp = %d WHERE cssid = %d LIMIT 1',
            array(time(),$val['cssid']));

    drupal_set_message(t('Saved style ID '. $val['styleid']));
    
    $sql = 'SELECT * FROM {style_edit_cssimport} WHERE cssid = %d';
    $result = db_query($sql,$val['cssid']);
    $cssinfo = db_fetch_array($result);

    $form_state['redirect'] = 'admin/settings/styleedit/list/'
                .$cssinfo['systemname'].'/editsheet/'.$val['cssid'];
}

function style_edit_stylesheet_edit_form($formstate, $theme, $cssid) {
    $styles = get_stylesheet_db($cssid);

    //Check for unsaved styles
    $result = db_query('SELECT * FROM {style_edit_cssimport} WHERE cssid = %d',$cssid);
    $cssinfo = db_fetch_array($result);
    if ($cssinfo['ufiletimestamp'] < $cssinfo['udbtimestamp'])
        drupal_set_message(t('You have unsaved changes!'), 'warning');

    $form = array();

    $form['styleitems'] = array();
    $form['styleitems']['#tree'] = TRUE;
    //$form['styleitems']['#theme'] = 'style_edit_stylesheet_edit_form';

    foreach ($styles as $key => $style) {

        if (count($style) < 2) continue;

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
            '#value' => l(t('edit'),'admin/settings/styleedit/list/'
                    .$theme.'/editstyle/'.$sid),
        );
        $form['styleitems'][$key]['sort'] = array(
            '#type' => 'weight',
            '#delta' => count($styles),
            '#default_value' => $sort,
        );
        $form['styleitems'][$key]['sid'] = array(
            '#input' => 'value',
            '#value' => $sid,
        );
    }

    $form['cssid'] = array(
        '#type' => 'hidden',
        '#value' => $cssid,
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

function style_edit_stylesheet_edit_form_submit($form, &$form_state) {
    //Update the database with the new values'
    foreach ($form_state['values']['styleitems'] as $item) {
        db_query('UPDATE {style_edit_cssstyles} SET weight = %d WHERE styleid = %d LIMIT 1',
                array($item['sort'], $item['sid']));
    }

    drupal_set_message(t('Saved stylesheet to database'));

    $time = save_stylesheet_to_fs($form_state['values']['cssid']);
    
    db_query('UPDATE {style_edit_cssimport} 
        SET udbtimestamp = %d, ufiletimestamp = %d
        WHERE cssid = %d LIMIT 1',
            array($time,$time,$form_state['values']['cssid']));
}

function theme_style_edit_stylesheet_edit_form($form){

    drupal_add_tabledrag('css_table', 'order', 'sibling', 'lsort');
    $header = array('',t('Selector'),t('Style'),t('Operation'),'sort');
    foreach (element_children($form['styleitems']) as $key) {

        $form['styleitems'][$key]['sort']['#attributes']['class'] = 'lsort';
        $form['styleitems'][$key]['itemid']['#attributes']['class'] = 'itemid';

        $row = array('');
        $row[] = drupal_render($form['styleitems'][$key]['selector']);
        $row[] = drupal_render($form['styleitems'][$key]['style']);
        $row[] = drupal_render($form['styleitems'][$key]['edit_op']);
        $row[] = drupal_render($form['styleitems'][$key]['sort']).
                    drupal_render($form['styleitems'][$key]['sid']);

        //Add a draggable class to every table row (<tr>)
        $rows[] = array('data' => $row, 'class' => 'draggable');
    }

    $output = theme('table', $header, $rows, array('id' => 'css_table'));

    $output .= drupal_render($form);

    return $output;
}


/**
 *  Load the Stylesheet from the database
 *
 * @param   int     $cssid  ID of the stylesheed to load
 * @return  array           List of items and count field
 */

function get_stylesheet_db($cssid) {
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

function get_stylesection_db($styleid) {
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

function save_stylesheet_to_fs($cssid) {

    $sql = "SELECT * FROM {style_edit_cssimport} WHERE cssid = %d";
    $result = db_query($sql,$cssid);
    $cssrow = db_fetch_array($result);

    $stylesections = get_stylesheet_db($cssid);

    $cssfile = $cssrow['path'];

    if (is_writable($cssfile)){
        $fhandle = fopen($cssfile, 'w+');
        foreach ($stylesections as $section) {

            if (count($section) < 2) continue;

            fwrite($fhandle, $section['selector']);
            fwrite($fhandle, $section['style']);
            fwrite($fhandle, chr(10).chr(13));
        }
        fclose($fhandle);
        drupal_set_message(t('Sucsessfull writen @file',array('@file' => basename($cssfile))));
    }else {
        drupal_set_message(t('Error while writing @file',array('@file' => basename($cssfile))));
    }

    return filemtime($cssrow['path']);
}

?>
