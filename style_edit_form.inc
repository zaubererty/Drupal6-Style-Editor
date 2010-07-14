<?php
// $Id$
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
*/

// Style editor
function style_edit_styleset_edit_form($formstate, $theme, $styleid) {

    // Form with values for text editor
    $form = set_text_editor_style_form_values(outline_text_edit_styleset(), $styleid);
    
    //Todo set bredcrump complete
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), '<front>');
    $breadcrumb[] = l(t('Style Editor'), 'admin/settings/styleedit');
    $breadcrumb[] = l($theme, 'admin/settings/styleedit/list/'.$theme);
    drupal_set_breadcrumb($breadcrumb);
    
    return $form;
}
/**
 * Implementation of hook_submit()
 * 
 * Update the database with the new values
 * 
 * @param $form         Current form 
 * @param $form_state   Submit state from the user
 */
function style_edit_styleset_edit_form_submit($form, &$form_state) {
    $val = $form_state['values'];
    switch ($val['sel_cat']['kind']) {
        case 'STYLE':
            $update_val = array($val['sel_cat']['selector'],'{'.$val['style']['css'].'}' ,$val['styleid']);
            break;
        case 'COMMENT':
            $update_val = array($val['sel_cat']['selector'],$val['style']['css'] ,$val['styleid']);
            break;
        default:
            drupal_set_message(t('Type not known!'), 'error');
            break;
    }
    db_query("UPDATE {style_edit_cssstyles} SET selector = '%s', style = '%s' WHERE styleid = %d LIMIT 1",
            $update_val);

    db_query('UPDATE {style_edit_cssimport} SET udbtimestamp = %d WHERE cssid = %d LIMIT 1',
            array(time(),$val['cssid']));

    drupal_set_message(t('Saved style ID '. $val['styleid']));
    
    $sql = 'SELECT * FROM {style_edit_cssimport} WHERE cssid = %d';
    $result = db_query($sql,$val['cssid']);
    $cssinfo = db_fetch_array($result);

    $form_state['redirect'] = 'admin/settings/styleedit/list/'
                .$cssinfo['systemname'].'/editsheet/'.$val['cssid'];
}


/**
 * Sets the values from data base for the text editor
 * 
 *  @param  $form       array   formoutline of text editor
 *  @param  $styleid    int     record id to load from 'style_edit_cssstyles'
 *  @return form with values    
 */
function set_text_editor_style_form_values($form, $styleid) {
    $data = get_stylesection_db($styleid);

    $form['sel_cat']['selector']['#default_value'] = $data['selector'];
    $form['sel_cat']['kind']['#default_value'] = $data['kind'];
    $form['style']['css']['#default_value'] = clear_brackets($data['style']);
    $form['styleid']['#value'] = $data['styleid'];
    $form['cssid']['#value'] = $data['cssid'];
    $form['edit_type']['#value'] = variable_set('style_edit_editor_type','text');
    
    return $form;
}

function outline_text_edit_styleset() {
    
    $form = array();
    
    $form['sel_cat'] = array(
            '#type' => 'fieldset',
            '#title' => t('Selector and category'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $form['sel_cat']['selector'] = array(
            '#type' => 'textfield',
            '#title' => t('Selector'),
    );

    $form['sel_cat']['kind'] = array(
            '#type' => 'select',
            '#title' => t('Type'),
            '#options' => array(
                'COMMENT' => t('Comment'),
                'STYLE' => t('Style'),
             ),
    );

    $form['style'] = array(
            '#type' => 'fieldset',
            '#title' => t('Styles'),
            '#tree' => TRUE,
            '#collapsible' => TRUE,
    );

    $form['style']['css'] = array(
            '#type' => 'textarea',
            '#title' => t('CSS'),
            '#rows' => 10,
    );

    $form['styleid'] = array(
            '#type' => 'value',
    );
    
    $form['cssid'] = array(
            '#type' => 'value',
    );

    $form['edit_type'] = array(
            '#type' => 'value',
    );
    
    $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
    );
    
    return $form;
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
    $table = array();
    while ($r = db_fetch_array($result)) {
        $table[] = $r;
    }
    $table['count'] = $result->num_rows;
    return $table;
}

//Stylesheet Form
function style_edit_stylesheet_edit_form($formstate, $theme, $cssid) {
    $styles = get_stylesheet_db($cssid);
    
    //Check for unsaved styles
    $result = db_query('SELECT * FROM {style_edit_cssimport} WHERE cssid = %d',$cssid);
    $cssinfo = db_fetch_array($result);
    if ($cssinfo['ufiletimestamp'] < $cssinfo['udbtimestamp'])
        drupal_set_message(t('You have unsaved changes!'), 'warning');

    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), '<front>');
    $breadcrumb[] = l(t('Style Editor'), 'admin/settings/styleedit');
    $breadcrumb[] = l($cssinfo['name'], 'admin/settings/styleedit/list/'.$theme);
    drupal_set_breadcrumb($breadcrumb);
        
    $form = array();

    $form['styleitems'] = array();
    $form['styleitems']['#tree'] = TRUE;
    
    $itemcount = $styles['count'];

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
            '#delta' => $itemcount,
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

    $form['submit_bottom'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    
    $form['submit_top'] = array(
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
  
    $output = drupal_render($form['submit_top']);

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

    $output .= theme('table', $header, $rows, array('id' => 'css_table'));

    $output .= drupal_render($form);

    return $output;
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
    return db_fetch_array($result);
}

/**
 * Cleans style on '{''}'
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


// Settings

/**
 * Implementation of style_edit_settings_form().
 *
 * @return An array containing the form elements to be displayed in the node edit form.
 */
function style_edit_settings_form($formstate) {
    $form = array();
    
    $form['edit_setting'] = array(
          '#type' => 'fieldset',
          '#title' => t('Editor settings'),
          '#tree' => TRUE,
          '#collapsible' => TRUE,
    );
    $form['edit_setting']['editor_type'] = array(
          '#type' => 'select', 
          '#title' => t('Select style editor type'), 
          '#default_value' => variable_get('style_edit_editor_type','text'),
          '#options' => array(
              'text' => t('Text editor'), 
              'assisted' => t('Formular based editor'), 
          ),
          '#description' => t('Global setting for the syleset editor type.'),
    );
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
  
    return $form;
}

/**
 * Implementation of style_edit_settings_form_submit().
 *
 */
function style_edit_settings_form_submit($form, &$form_state) {
    $val = $form_state['values'];
    variable_set('style_edit_editor_type','text');
    drupal_set_message(t('Settings saved'));
    drupal_goto('admin/settings/styleedit/list/');
}

?>
